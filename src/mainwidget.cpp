#include "mainwidget.h"
#include "view/imageview.h"
#include "loadingwidget.h"
#include "frame.h"

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QTextBlock>
#include <QStandardPaths>
#include <QFileDialog>
#include <QFileInfo>
#include <QThread>
#include <QMutexLocker>
#include <QSplitter>
#include <QTimer>
#include <QShortcut>

#include <DGuiApplicationHelper>
#include <DMainWindow>
#include <DTitlebar>
#include <DMessageManager>
#include <DToolButton>
#include <DFloatingWidget>
#include <DAnchors>
#include <DFontSizeManager>
#include <DHiDPIHelper>

#define App (static_cast<QApplication*>(QCoreApplication::instance()))
MainWidget::MainWidget(QWidget *parent) :
    DWidget(parent)
{
    setupUi(this);
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    setIcons(themeType);
    initScaleLabel();
    setupConnect();
}

MainWidget::~MainWidget()
{
    //程序即将结束,线程标志结束
    m_isEndThread = 0;

    // 等待线程结束
    if (m_loadImagethread) {
        m_loadImagethread->requestInterruption(); // 请求线程中断
        if (!m_loadImagethread->wait(3000)) { // 等待线程结束，超时时间为3秒
            qWarning() << "OCR thread TIMEOUT";
            m_loadImagethread->terminate(); // 如果线程没有结束，强制终止线程
            m_loadImagethread->deleteLater(); // 删除线程对象
            m_loadImagethread = nullptr;
        }
        // 正常结束的情况下delete操作在槽函数中执行
    }

//    m_mainGridLayout->addLayout(m_buttonHorizontalLayout, 1, 0, 1, 1);
    if (m_mainGridLayout && m_buttonHorizontalLayout) {
        m_mainGridLayout->removeItem(m_buttonHorizontalLayout);
    }
    if (m_imageview) {
        m_imageview->deleteLater();
        m_imageview = nullptr;
    }
    if (m_mainGridLayout) {
        m_mainGridLayout->deleteLater();
        m_mainGridLayout = nullptr;
    }
    if (m_horizontalLayout) {
        m_horizontalLayout->deleteLater();
        m_horizontalLayout = nullptr;
    }
    if (m_plainTextEdit) {
        m_plainTextEdit->deleteLater();
        m_plainTextEdit = nullptr;
    }
    if (m_buttonHorizontalLayout) {
        m_buttonHorizontalLayout->deleteLater();
        m_buttonHorizontalLayout = nullptr;
    }
    if (m_tipHorizontalLayout) {
        m_tipHorizontalLayout->deleteLater();
        m_tipHorizontalLayout = nullptr;
    }
    if (m_tiplabel) {
        m_tiplabel->deleteLater();
        m_tiplabel = nullptr;
    }
    if (m_copyBtn) {
        m_copyBtn->deleteLater();
        m_copyBtn = nullptr;
    }
    if (m_exportBtn) {
        m_exportBtn->deleteLater();
        m_exportBtn = nullptr;
    }
    if (m_tipIconLabel) {
        m_tipIconLabel->deleteLater();
        m_tipIconLabel = nullptr;
    }

}

void MainWidget::setupUi(QWidget *Widget)
{
    DMainWindow *mainWindow = static_cast<DMainWindow *>(this->parent());
    if (mainWindow) {
        mainWindow->titlebar()->setMenuVisible(false);
    }
    m_mainGridLayout = new QGridLayout(Widget);
    m_mainGridLayout->setSpacing(0);
    m_mainGridLayout->setContentsMargins(0, 0, 0, 6);
    m_mainGridLayout->setObjectName(QStringLiteral("gridLayout"));

    m_horizontalLayout = new QHBoxLayout(Widget);
    m_horizontalLayout->setSpacing(0);
    m_horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

    m_plainTextEdit = new ResultTextView(Widget);
    m_plainTextEdit->setLineWidth(0);
//    m_plainTextEdit->setWindowFlags(Qt::FramelessWindowHint);
//    m_plainTextEdit->setStyleSheet("background:transparent;border:2px solid red;");

    m_plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

    m_frame = new QFrame(this);
    m_frame->setObjectName(QStringLiteral("m_frame"));
    m_frame->setFrameShape(QFrame::StyledPanel);
    m_frame->setFrameShadow(QFrame::Raised);
    m_frame->setLineWidth(0);
    m_frameLayout = new QHBoxLayout(m_frame);
    m_frameLayout->setSpacing(6);
    m_frameLayout->setContentsMargins(11, 11, 11, 11);
    m_frameLayout->setObjectName(QStringLiteral("horizontalLayout"));
    m_frameLayout->setContentsMargins(10, 10, 10, 10);

    //后面加一层窗口,左边空间间距20
    m_frameStack = new Frame(this);
    m_frameStack->setFrameShape(QFrame::StyledPanel);
    m_frameStack->setFrameShadow(QFrame::Raised);
    m_frameStack->setLineWidth(0);
    m_frameStackLayout = new QHBoxLayout(m_frameStack);
    m_frameStackLayout->setSpacing(0);
    m_frameStackLayout->setObjectName(QStringLiteral("horizontalLayoutStack"));
    m_frameStackLayout->setContentsMargins(20, 0, 5, 0);

    m_imageview = new ImageView(m_frame);
//    m_imageview->setObjectName(QStringLiteral("m_imageView"));
    m_imageview->setLineWidth(0);

    m_frameLayout->addWidget(m_imageview);
//    if (!m_imageview) {
//        m_imageview = new ImageView();
//        m_imageview->setLineWidth(0);
//    }

    m_resultWidget = new DStackedWidget(this);
    m_resultWidget->setFocusPolicy(Qt::NoFocus);

    m_frameStackLayout->addWidget(m_resultWidget);
    m_frameStack->setMinimumWidth(220);
    //宽度最大值为440
    m_frameStack->setMaximumWidth(440);

    connect(m_frameStack, &Frame::sigFrameResize, this, [ = ] {
        m_resultWidget->resize(QSize(m_frameStack->width() - 25, m_frameStack->height()));
    });


    m_loadingOcr = new loadingWidget(this);
    m_loadingOcr->setAlignment(Qt::AlignCenter);
    m_resultWidget->addWidget(m_loadingOcr);

    m_resultWidget->addWidget(m_plainTextEdit);

    m_noResult = new DLabel(this);
    m_noResult->setAlignment(Qt::AlignCenter);
    m_noResult->setText(tr("No text recognized"));
    m_resultWidget->addWidget(m_noResult);

    connect(m_loadingOcr, &loadingWidget::sigChangeSize, [ = ] {
        loadingUi();
    });
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal); //新建水平分割器
    mainSplitter->setHandleWidth(0);//分割线的宽度
    mainSplitter->setChildrenCollapsible(false);//不允许把分割出的子窗口拖小到0，最小值被限定为sizeHint或maxSize/minSize
    mainSplitter->addWidget(m_frame);//把ui中拖出的各个控件拿走，放到分割器里面
    mainSplitter->addWidget(m_frameStack);
    QList<int> list;
    //默认右侧识栏目是220的宽度
    list << 620 << 220;
    mainSplitter->setSizes(list);
//    m_horizontalLayout->addWidget(m_imageview);
//    m_horizontalLayout->addWidget(m_plainTextEdit);
    m_horizontalLayout->addWidget(mainSplitter);
    m_horizontalLayout->setStretch(0, 1);

    m_mainGridLayout->addLayout(m_horizontalLayout, 0, 0, 1, 1);
    m_mainGridLayout->setColumnStretch(0, 1);

    m_buttonHorizontalLayout = new QHBoxLayout(Widget);
    m_buttonHorizontalLayout->setContentsMargins(20, 3, 59, 3); //表示控件与窗体的左右边距
//    m_buttonHorizontalLayout->setSpacing(30);


    m_tipHorizontalLayout = new QHBoxLayout(Widget);

    m_tipIconLabel = new DLabel(Widget);
    m_tipIconLabel->setObjectName(QStringLiteral("tipIconLabel"));

    m_tipHorizontalLayout->addWidget(m_tipIconLabel);

    m_tiplabel = new DLabel(Widget);
    m_tiplabel->setObjectName(QStringLiteral("tiplabel"));

    m_tipHorizontalLayout->setSpacing(4);
    m_tipHorizontalLayout->addWidget(m_tiplabel);

    m_buttonHorizontalLayout->addLayout(m_tipHorizontalLayout);
    QSpacerItem *horizontalSpacer = new QSpacerItem(159, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_buttonHorizontalLayout->addItem(horizontalSpacer);

    m_copyBtn = new DToolButton(Widget);
    m_copyBtn->setObjectName(QStringLiteral("Copy text"));
    m_copyBtn->setMaximumSize(QSize(36, 36));
    m_copyBtn->setToolTip(tr("Copy text"));

    m_buttonHorizontalLayout->addWidget(m_copyBtn);
    m_buttonHorizontalLayout->setSpacing(30);
    m_exportBtn = new DToolButton(Widget);
    m_exportBtn->setObjectName(QStringLiteral("Save as TXT"));
    m_exportBtn->setMaximumSize(QSize(36, 36));
    m_exportBtn->setToolTip(tr("Save as TXT"));

    m_buttonHorizontalLayout->addWidget(m_exportBtn);

    m_line = new DHorizontalLine(this);
    m_line->setObjectName(QStringLiteral("m_line"));

    m_mainGridLayout->addWidget(m_line, 1, 0, 1, 1);

    m_mainGridLayout->addLayout(m_buttonHorizontalLayout, 2, 0, 1, 1);


    retranslateUi(Widget);

    QMetaObject::connectSlotsByName(Widget);
    m_pwidget = new QWidget(this);
    m_pwidget->setFocusPolicy(Qt::NoFocus);
    m_pwidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_pwidget->setFixedSize(this->width(), this->height() - 23);
    m_pwidget->move(0, 0);


}

void MainWidget::setupConnect()
{
    initShortcut();
    //增加适配运行时的风格切换
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MainWidget::setIcons);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &MainWidget::setIcons);
    connect(m_exportBtn, &DIconButton::clicked, this, &MainWidget::slotExport);
    connect(m_copyBtn, &DIconButton::clicked, this, &MainWidget::slotCopy);
    connect(this, &MainWidget::sigResult, this, [ = ](const QString & result) {
        loadString(result);
        deleteLoadingUi();
    });
}

void MainWidget::retranslateUi(QWidget *Widget)
{
//    Widget->setWindowTitle("Widget");
    m_tiplabel->setText(QApplication::translate("Widget", "Tips: The clearer the image is, the more accurate the text is", nullptr));
//    m_copyBtn->setText(QApplication::translate("Widget", "Copy", nullptr));
    //    m_exportBtn->setText(QApplication::translate("Widget", "Export", nullptr));
}

void MainWidget::createLoadingUi()
{
    m_isLoading = true;
    m_loadingWidget = new TextLoadWidget(this);
    m_loadingWidget->resize(40, 40);
    m_loadingWidget->show();
    m_loadingTip = new DLabel(tr("Recognizing"), this);
    m_loadingTip->show();
//    if (m_imageview) {
//        m_imageview->setForegroundBrush(QColor(0, 0, 0, 77)); //设置场景的前景色，类似于遮罩
//    }
    //识别时
    if (m_copyBtn) {
        m_copyBtn->setEnabled(false);
    }
    if (m_exportBtn) {
        m_exportBtn->setEnabled(false);
    }
}

void MainWidget::deleteLoadingUi()
{
    m_isLoading = false;
    if (m_loadingWidget) {
        m_loadingWidget->deleteLater();
        m_loadingWidget = nullptr;
    }
    if (m_loadingTip) {
        m_loadingTip->deleteLater();
        m_loadingTip = nullptr;
    }
    m_imageview->setForegroundBrush(QColor(0, 0, 0, 0)); //设置场景的前景色，类似于遮罩
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    setIcons(themeType);
}

void MainWidget::loadingUi()
{
    if (m_loadingWidget && m_loadingTip && m_frameStack) {
        int x = this->width() - m_frameStack->width() / 2;
        int y = this->height() / 2 - 50;
        qDebug() << m_loadingWidget->width();
        m_loadingWidget->setFixedSize(QSize(24, 24));
        m_loadingWidget->move(x - m_loadingWidget->width() / 2, y);
        qDebug() << m_loadingTip->width();
//        m_loadingTip->setAlignment(Qt::AlignCenter);
        QFontMetrics fm(DFontSizeManager::instance()->get(DFontSizeManager::T8));
        int mPoint = fm.boundingRect(tr("Recognizing")).width();
        m_loadingTip->move(x - mPoint / 2 - 4, y + 24);
    }
    if (m_pwidget) {
        m_pwidget->setFixedSize(this->width(), this->height() - 48);
        m_pwidget->move(0, 0);
    }
}

void MainWidget::initShortcut()
{
    m_scAddView = new QShortcut(QKeySequence("Ctrl+="), this);
    m_scAddView->setContext(Qt::WindowShortcut);
    connect(m_scAddView, &QShortcut::activated, this, [ = ] {
        if (m_imageview)
        {
            m_imageview->setScaleValue(1.1);
        }
    });

    m_scReduceView = new QShortcut(QKeySequence("Ctrl+-"), this);
    m_scReduceView->setContext(Qt::WindowShortcut);
    connect(m_scReduceView, &QShortcut::activated, this, [ = ] {
        if (m_imageview)
        {
            m_imageview->setScaleValue(0.9);
        }
    });

}

bool MainWidget::openImage(const QString &path)
{
    bool bRet = false;
    if (m_imageview) {
        QImage img(path);
        if (!img.isNull()) {
            m_imgName = path;
            openImage(img, m_imgName);
            bRet = true;
        }
    }
    return bRet;
}

void MainWidget::openImage(const QImage &img, const QString &name)
{
    createLoadingUi();
    //新打开的窗口需要设置属性
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    setIcons(themeType);
    if (m_imageview) {
        m_imageview->openFilterImage(img);
        // 使用 QPointer 检查对象是否还存在
        QPointer<ImageView> imgSelf(m_imageview);
        QTimer::singleShot(100, [ = ] {
            if (imgSelf.isNull()) {
                qWarning() << "ImageView obj is destroyed";
                return; // 对象已被销毁
            }

            //分辨率大于window的采用适应窗口，没超过，则适应图片
            QRect rect1 = m_imageview->image().rect();
            if ((rect1.width() >= m_imageview->width() || rect1.height() >= m_imageview->height() - 150) && m_imageview->width() > 0 &&
                    height() > 0)
            {
                m_imageview->fitWindow();
            } else
            {
                m_imageview->fitImage();
            }
        });
        m_imgName = name;
    }
    if (m_currentImg) {
        delete m_currentImg;
        m_currentImg = nullptr;
    }
    m_currentImg = new QImage(img);
    if (!m_loadImagethread) {
        m_loadImagethread = QThread::create([ = ]() {
            QMutexLocker locker(&m_mutex);
            m_result = PaddleOCRApp::instance()->getRecogitionResult(*m_currentImg);
            //判断程序是否退出
            if (1 == m_isEndThread) {
                emit sigResult(m_result);
            }
        });
    }
    connect(m_loadImagethread, &QThread::finished, [this]() {
        m_loadImagethread->deleteLater();
        m_loadImagethread = nullptr;
    });
    m_loadImagethread->start();
}

void MainWidget::loadHtml(const QString &html)
{
    if (!html.isEmpty()) {
        m_frameStackLayout->setContentsMargins(20, 0, 5, 0);
        m_resultWidget->setCurrentWidget(m_plainTextEdit);
        m_plainTextEdit->appendHtml(html);
//        m_plainTextEdit->setHtml(html);
    } else {
        resultEmpty();
    }
}

void MainWidget::loadString(const QString &string)
{
    if (!string.isEmpty()) {
        m_frameStackLayout->setContentsMargins(20, 0, 5, 0);
        m_resultWidget->setCurrentWidget(m_plainTextEdit);
        m_plainTextEdit->appendPlainText(string);
//        m_plainTextEdit->setText(string);
        //读取完了显示在最上方
        m_plainTextEdit->moveCursor(QTextCursor::Start) ;
        m_plainTextEdit->ensureCursorVisible() ;
        //新增识别完成按钮恢复
        if (m_copyBtn) {
            m_copyBtn->setEnabled(true);
        }
        if (m_exportBtn) {
            m_exportBtn->setEnabled(true);
        }
    } else {
        resultEmpty();
    }
}

void MainWidget::resultEmpty()
{
    //修复未识别到文字没有居中对齐的问题
    m_frameStackLayout->setContentsMargins(20, 0, 20, 0);
    m_resultWidget->setCurrentWidget(m_noResult);
    //新增如果未识别到，按钮置灰
    if (m_copyBtn) {
        m_copyBtn->setEnabled(false);
    }
    if (m_exportBtn) {
        m_exportBtn->setEnabled(false);
    }
}

void MainWidget::initScaleLabel()
{
    DAnchors<DFloatingWidget> scalePerc = new DFloatingWidget(this);
    scalePerc->setBlurBackgroundEnabled(true);

    QHBoxLayout *layout = new QHBoxLayout();
    scalePerc->setLayout(layout);
    DLabel *label = new DLabel();
    layout->addWidget(label);
    scalePerc->setAttribute(Qt::WA_TransparentForMouseEvents);
    scalePerc.setAnchor(Qt::AnchorHorizontalCenter, this, Qt::AnchorHorizontalCenter);
    scalePerc.setAnchor(Qt::AnchorBottom, this, Qt::AnchorBottom);
    scalePerc.setBottomMargin(75 + 14);
    label->setAlignment(Qt::AlignCenter);
//    scalePerc->setFixedSize(82, 48);
    scalePerc->setFixedWidth(90 + 10);
    scalePerc->setFixedHeight(40 + 10);
    scalePerc->adjustSize();
    label->setText("100%");
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T6);
    scalePerc->hide();

    QTimer *hideT = new QTimer(this);
    hideT->setSingleShot(true);
    connect(hideT, &QTimer::timeout, scalePerc, &DLabel::hide);

    connect(m_imageview, &ImageView::scaled, this, [ = ](qreal perc) {
        label->setText(QString("%1%").arg(int(perc)));
    });
    connect(m_imageview, &ImageView::showScaleLabel, this, [ = ]() {
        scalePerc->show();
        scalePerc->move(m_imageview->width() / 2 - 50, m_imageview->height() - 48);
        hideT->start(1000);
    });
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    return DWidget::resizeEvent(event);
}

void MainWidget::paintEvent(QPaintEvent *event)
{
    loadingUi();
    return DWidget::paintEvent(event);
}


void MainWidget::slotCopy()
{
    //选中内容则复制，未选中内容则不复制
    if (!m_plainTextEdit->textCursor().selectedText().isEmpty()) {
        m_plainTextEdit->copy();
    } else {
        QTextDocument *document = m_plainTextEdit->document();
        QPlainTextEdit *tempTextEdit = new QPlainTextEdit(this);
        tempTextEdit->setDocument(document);
        tempTextEdit->selectAll();
        tempTextEdit->copy();
        tempTextEdit->deleteLater();
        tempTextEdit = nullptr;
    }

    QIcon icon(":/assets/icon_toast_sucess_new.svg");
    DFloatingMessage *pDFloatingMessage = new DFloatingMessage(DFloatingMessage::MessageType::TransientType, m_pwidget);
    pDFloatingMessage->setBlurBackgroundEnabled(true);
    pDFloatingMessage->setMessage(tr("Copied"));
    pDFloatingMessage->setIcon(icon);
    pDFloatingMessage->raise();
    DMessageManager::instance()->sendMessage(m_pwidget, pDFloatingMessage);

}

void MainWidget::slotExport()
{
    QStringList list = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
    QString download = "";
    if (list.size() > 0) {
        download = list.at(0);
    } else {
        QStringList home = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
        if (home.size() > 0) {
            download = home.at(0) + "/Downloads";
        }
    }

    QString fileName;
    if (!m_imgName.isEmpty()) {
        fileName = QFileInfo(m_imgName).completeBaseName();
    } else {
        fileName = "Results";
    }

    QString file_path = QFileDialog::getSaveFileName(this, "save as", download + "/" + fileName, "*.txt");
    qDebug() << file_path;

    //这里不应该增加一个.txt，会导致有两个后缀
    QString path = file_path;
    QFile file(path);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << m_plainTextEdit->document()->toPlainText();
    }
}

void MainWidget::setIcons(DGuiApplicationHelper::ColorType themeType)
{
    if (themeType == DGuiApplicationHelper::DarkType) {
        QPalette pal;
        pal.setColor(QPalette::Background, QColor(32, 32, 32, 179));
        setAutoFillBackground(true);
        setPalette(pal);
        if (m_resultWidget) {
            QPalette pal;
            pal.setColor(QPalette::Background, QColor(40, 40, 40));
            m_resultWidget->setAutoFillBackground(true);
            m_resultWidget->setPalette(pal);
            //增加frame的颜色设置
            m_frameStack->setAutoFillBackground(true);
            m_frameStack->setPalette(pal);
        }
        if (m_tipIconLabel) {
            QPixmap m_tipImage = DHiDPIHelper::loadNxPixmap(":/assets/tip_dark.svg");
            m_tipIconLabel->setPixmap(m_tipImage);
            m_tipIconLabel->setFixedSize(QSize(14, 14));
        }

        if (m_copyBtn) {
            QPixmap m_shadowImg = DHiDPIHelper::loadNxPixmap(":/mpimage/light/shadow.svg");
            m_copyBtn->setIcon(QIcon(":/assets/copy_dark.svg"));
            m_copyBtn->setIconSize(QSize(36, 36));
        }
        if (m_exportBtn) {
            m_exportBtn->setIcon(QIcon(":/assets/download_dark.svg"));
            m_exportBtn->setIconSize(QSize(36, 36));
        }

        App->setWindowIcon(QIcon(":/assets/appicon_dark.svg"));
        DMainWindow *mainWindow = static_cast<DMainWindow *>(this->parent());
        if (mainWindow) {
            mainWindow->titlebar()->setIcon(QIcon(":/assets/appicon_dark.svg"));
        }
        if (m_isLoading && m_imageview) {
            m_imageview->setForegroundBrush(QColor(0, 0, 0, 127)); //设置场景的前景色，类似于遮罩
            m_imageview->setBackgroundBrush(QColor(34, 34, 34));
            QPalette pal;
            pal.setColor(QPalette::Background, QColor(0, 0, 0, 125));
            m_frame->setAutoFillBackground(true);
            m_frame->setPalette(pal);
        } else if (m_imageview) {
            m_imageview->setBackgroundBrush(QColor(35, 35, 35));
            QPalette pal;
            pal.setColor(QPalette::Background, QColor(35, 35, 35));
            m_frame->setAutoFillBackground(true);
            m_frame->setPalette(pal);
        }
        if (m_tiplabel) {
            QPalette pe;
            pe.setColor(QPalette::WindowText, QColor(109, 124, 136));
            m_tiplabel->setPalette(pe);
        }
        if (m_noResult) {
            QPalette pe;
            pe.setColor(QPalette::WindowText, QColor(192, 198, 212, 102));
            m_noResult->setPalette(pe);
        }
        if (m_loadingTip) {
            QPalette pe;
            pe.setColor(QPalette::WindowText, QColor(192, 198, 212));
            m_loadingTip->setPalette(pe);
        }

    } else {
        QPalette pal;
        pal.setColor(QPalette::Background, QColor(255, 255, 255, 179));
        setAutoFillBackground(true);
        setPalette(pal);
        //修复因为切换导致的颜色差
        if (m_resultWidget && m_frameStack) {
            QPalette pal;
            pal.setColor(QPalette::Background, QColor(255, 255, 255));
            m_resultWidget->setAutoFillBackground(true);
            m_resultWidget->setPalette(pal);
            //增加frame的颜色设置
            m_frameStack->setAutoFillBackground(true);
            m_frameStack->setPalette(pal);
        }
        if (m_tipIconLabel) {
            QPixmap m_tipImage = DHiDPIHelper::loadNxPixmap(":/assets/tip_light.svg");
            m_tipIconLabel->setPixmap(m_tipImage);
//            m_tipIconLabel->setPixmap(QPixmap(":/assets/tip_light.svg"));
            m_tipIconLabel->setFixedSize(QSize(14, 14));
        }

        if (m_copyBtn) {
            m_copyBtn->setIcon(QIcon(":/assets/copy_light.svg"));
            m_copyBtn->setIconSize(QSize(36, 36));
        }
        if (m_exportBtn) {
            m_exportBtn->setIcon(QIcon(":/assets/download_light.svg"));
            m_exportBtn->setIconSize(QSize(36, 36));
        }

        App->setWindowIcon(QIcon(":/assets/appicon_light.svg"));
        DMainWindow *mainWindow = static_cast<DMainWindow *>(this->parent());
        if (mainWindow) {
            mainWindow->titlebar()->setIcon(QIcon(":/assets/appicon_light.svg"));
        }
        if (m_isLoading && m_imageview) {
            m_imageview->setForegroundBrush(QColor(0, 0, 0, 77)); //设置场景的前景色，类似于遮罩
            m_imageview->setBackgroundBrush(QColor(248, 248, 248));
            QPalette pal;
            pal.setColor(QPalette::Background, QColor(172, 172, 172));
            m_frame->setAutoFillBackground(true);
            m_frame->setPalette(pal);
        } else if (m_imageview) {
            //修改为正确的背景setBackgroundBrush
            m_imageview->setBackgroundBrush(QColor(248, 248, 248));
            QPalette pal;
            pal.setColor(QPalette::Background, QColor(248, 248, 248));
            m_frame->setAutoFillBackground(true);
            m_frame->setPalette(pal);
        }
        if (m_tiplabel) {
            QPalette pe;
            pe.setColor(QPalette::WindowText, QColor(138, 161, 180));
            m_tiplabel->setPalette(pe);

        }
        if (m_noResult) {
            QPalette pe;
            pe.setColor(QPalette::WindowText, QColor(85, 85, 85, 102));
            m_noResult->setPalette(pe);
        }
        if (m_loadingTip) {
            QPalette pe;
            pe.setColor(QPalette::WindowText, QColor(65, 77, 104));
            m_loadingTip->setPalette(pe);
        }

    }
} // setupUi

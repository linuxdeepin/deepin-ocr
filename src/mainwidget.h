#ifndef WIDGET_H
#define WIDGET_H

#include <DWidget>
#include <DIconButton>
#include <DPlainTextEdit>
#include <DLabel>
#include <DGuiApplicationHelper>
#include <DStackedWidget>

#include <QMutex>

#include <DToolButton>

#include "resulttextview.h"
#include "textloadwidget.h"
#include "tessocrutils/tessocrutils.h"

class QThread;
class QGridLayout;
class QHBoxLayout;
class ImageView;
class loadingWidget;
class QShortcut;
DWIDGET_USE_NAMESPACE

class MainWidget : public DWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void setupUi(QWidget *Widget);
    void setupConnect();
    void retranslateUi(QWidget *Widget);

    void createLoadingUi();
    void deleteLoadingUi();
    void loadingUi();
    //初始化快捷键
    void initShortcut();

    void openImage(const QString &path);
    void openImage(const QImage &img, const QString &name = "");

    void loadHtml(const QString &html);
    void loadString(const QString &string);
    void resultEmpty();

    //缩放显示label
    void initScaleLabel();
protected:
    void resizeEvent(QResizeEvent *event);
private slots:
    void setIcons(DGuiApplicationHelper::ColorType themeType);

    void slotCopy();
    void slotExport();

//    void change()
private:
    QGridLayout *m_mainGridLayout{nullptr};
    QHBoxLayout *m_horizontalLayout{nullptr};
    ResultTextView *m_plainTextEdit{nullptr};
    QHBoxLayout *m_buttonHorizontalLayout{nullptr};
    QHBoxLayout *m_tipHorizontalLayout{nullptr};
    DLabel *m_tiplabel{nullptr};
    DToolButton *m_copyBtn{nullptr};
    DToolButton *m_exportBtn{nullptr};
    ImageView *m_imageview{nullptr};
    DLabel *m_tipIconLabel{nullptr};

    QString m_imgName;  //当前图片绝对路径
    QWidget *m_pwidget{nullptr};
    TextLoadWidget *m_loadingWidget{nullptr};
    DLabel *m_loadingTip{nullptr};

    bool m_isLoading{false};

    QThread *m_loadImagethread{nullptr};
    QMutex m_mutex;
    RecognitionResult m_result;
    QImage *m_currentImg{nullptr};

    DStackedWidget *m_resultWidget{nullptr};
    DLabel *m_noResult{nullptr};
    loadingWidget *m_loadingOcr{nullptr};
    QShortcut *m_scAddView = nullptr;
    QShortcut *m_scReduceView = nullptr;
signals:
    void sigResult(const QString &);

};

#endif // WIDGET_H

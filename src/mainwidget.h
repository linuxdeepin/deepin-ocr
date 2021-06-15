#ifndef WIDGET_H
#define WIDGET_H

#include <DWidget>
#include <DIconButton>
#include <DPlainTextEdit>
#include <DLabel>
#include <DGuiApplicationHelper>

#include <QMutex>

#include "resulttextview.h"
#include "textloadwidget.h"
#include "tessocrutils/tessocrutils.h"

class QThread;
class QGridLayout;
class QHBoxLayout;
class ImageView;

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


    void openImage(const QString &path);
    void openImage(const QImage &img);

    void loadHtml(const QString &html);
    void loadString(const QString &string);

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
    DIconButton *m_copyBtn{nullptr};
    DIconButton *m_exportBtn{nullptr};
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
};

#endif // WIDGET_H

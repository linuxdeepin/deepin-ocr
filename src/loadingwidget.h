#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <DLabel>
DWIDGET_USE_NAMESPACE

class loadingWidget : public DLabel
{
    Q_OBJECT
public:
    loadingWidget(QWidget *parent = nullptr);
signals:
    void sigChangeSize();
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // LOADINGWIDGET_H

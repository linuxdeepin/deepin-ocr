#include "loadingwidget.h"

loadingWidget::loadingWidget(QWidget *parent)
    : DLabel(parent)
{

}

void loadingWidget::resizeEvent(QResizeEvent *event)
{
    emit sigChangeSize();
    return DLabel::resizeEvent(event);
}

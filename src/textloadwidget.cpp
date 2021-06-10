#include "textloadwidget.h"
#include <QVBoxLayout>

TextLoadWidget::TextLoadWidget(DWidget *parent)
    : DWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    m_spinner = new DSpinner(this);
    m_spinner->start();
    m_spinner->setFixedSize(24, 24);
    m_spinner->show();
}

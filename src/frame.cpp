#include "frame.h"

Frame::Frame(QWidget *parent, Qt::WindowFlags f)
    : QFrame(parent, f)
{

}

void Frame::resizeEvent(QResizeEvent *event)
{
    emit sigFrameResize();
    return QFrame::resizeEvent(event);
}

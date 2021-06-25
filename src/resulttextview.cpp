#include "resulttextview.h"
#include <QDebug>
#include <QScroller>
#include <QClipboard>
#include <QApplication>

ResultTextView::ResultTextView(QWidget *parent)
    : m_Menu(nullptr), m_actCopy(nullptr), m_actCut(nullptr), m_actSelectAll(nullptr)
{
    Q_UNUSED(parent)
    //鼠标滑动事件接入
    QScroller::grabGesture(this->viewport(), QScroller::ScrollerGestureType::TouchGesture);
    m_Menu = new QMenu(this);
    m_actCopy = new QAction(this);
    m_actCopy->setText(tr("Copy"));
    m_actCut = new QAction(this);
    m_actCut->setText(tr("Cut"));
    m_actPaste = new QAction(this);
    m_actPaste->setText(tr("Paste"));
    m_actSelectAll = new QAction(this);
    m_actSelectAll->setText(tr("SelectAll"));

    m_Menu->addAction(m_actSelectAll);
    m_Menu->addAction(m_actCopy);
    m_Menu->addAction(m_actPaste);
    m_Menu->addAction(m_actCut);

    connect(m_actSelectAll, &QAction::triggered, this, [ = ]() {
        emit this->selectAll();
    });
    connect(m_actCopy, &QAction::triggered, this, [ = ]() {
        emit this->copy();
    });
    connect(m_actCut, &QAction::triggered, this, [ = ]() {
        emit this->cut();
    });
    connect(m_actPaste, &QAction::triggered, this, [ = ]() {
        emit this->paste();
    });
}

void ResultTextView::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
    //当前是否有选中文本
    QString select_str = this->textCursor().selectedText();
    if (select_str.length() < 1) {
        m_actCopy->setEnabled(false);
        m_actCut->setEnabled(false);
    } else {
        m_actCopy->setEnabled(true);
        m_actCut->setEnabled(true);
    }
    //判断剪切板的状态
    QString clip_str = QApplication::clipboard()->text();
    if (clip_str.isEmpty()) {
        m_actPaste->setEnabled(false);
    } else {
        m_actPaste->setEnabled(true);
    }
    m_Menu->exec(QCursor::pos());
}

void ResultTextView::resizeEvent(QResizeEvent *event)
{
    emit sigChangeSize();
    DPlainTextEdit::resizeEvent(event);
}

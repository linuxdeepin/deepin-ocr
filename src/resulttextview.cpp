#include "resulttextview.h"
#include <QDebug>

ResultTextView::ResultTextView(QWidget *parent)
    : m_Menu(nullptr), m_actCopy(nullptr), m_actCut(nullptr), m_actSelectAll(nullptr)
{
    Q_UNUSED(parent)
    m_Menu = new QMenu(this);
    m_actCopy = new QAction(this);
    m_actCopy->setText(tr("Copy"));
    m_actCut = new QAction(this);
    m_actCut->setText(tr("Cut"));
    m_actSelectAll = new QAction(this);
    m_actSelectAll->setText(tr("SelectAll"));

    m_Menu->addAction(m_actSelectAll);
    m_Menu->addAction(m_actCopy);
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
    m_Menu->exec(QCursor::pos());
}

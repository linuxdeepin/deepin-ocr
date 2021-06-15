#ifndef RESULTTEXTVIEW_H
#define RESULTTEXTVIEW_H

#include <QObject>
#include <DPlainTextEdit>
#include <QAction>
#include <QMenu>

DWIDGET_USE_NAMESPACE

class ResultTextView : public DPlainTextEdit
{
    Q_OBJECT
public:
    explicit ResultTextView(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void sigChangeSize();
private:
    QMenu *m_Menu;
    QAction *m_actCopy;
    QAction *m_actCut;
    QAction *m_actSelectAll;
};

#endif // RESULTTEXTVIEW_H

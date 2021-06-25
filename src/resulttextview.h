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
    QMenu *m_Menu{nullptr};
    QAction *m_actCopy{nullptr};
    QAction *m_actCut{nullptr};
    QAction *m_actPaste{nullptr};
    QAction *m_actSelectAll{nullptr};
};

#endif // RESULTTEXTVIEW_H

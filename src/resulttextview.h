#ifndef RESULTTEXTVIEW_H
#define RESULTTEXTVIEW_H

#include <QObject>
#include <QTextEdit>
#include <QAction>
#include <QMenu>


class ResultTextView : public QTextEdit
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

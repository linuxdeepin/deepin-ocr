// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 *图像显示界面
*/
#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>

#include "engine/OcrResult.h"

class QGraphicsPixmapItem;
class QGestureEvent;
class QPinchGesture;
class QMenu;

class ImageView : public QGraphicsView
{
    Q_OBJECT
public:
    ImageView(QWidget *parent = nullptr);
    ~ImageView();
    //通过路径打开图片
    void openImage(const QString &path);

    //用于鼠标滚轮滑动
    qreal windowRelativeScale() const;
    qreal imageRelativeScale() const;
    void scaleAtPoint(QPoint pos, qreal factor);
    void setScaleValue(qreal v);
    //自适应窗口
    void autoFit();
    //鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    bool event(QEvent *event)override;
    bool gestureEvent(QGestureEvent *event);

    //返回当前图片img
    const QImage image();
    void openFilterImage(QImage img);

    void setOcrResult(const OcrResult &result);
    void clearOcrResult();
    void selectAllText();
    void selectRange(int start, int end);
    bool hasSelection() const;
    QString selectedText() const;
    QList<QPair<int, int>> selectionRanges() const;
public slots:
    //适应窗口大小
    void fitWindow();
    //适应图片大小
    void fitImage();
    //旋转图片，感觉index角度，-为左，+为右
    void RotateImage(const int &index);

    //窗口大小改变事件
    void resizeEvent(QResizeEvent *event) override;
    //鼠标滚轮事件
    void wheelEvent(QWheelEvent *event) override;
protected:
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    //二指捏合功能的触屏事件
    void handleGestureEvent(QGestureEvent *gesture);
    void pinchTriggered(QPinchGesture *gesture);
signals:
    void scaled(qreal perc);
    void showScaleLabel();
    void selectionRangesChanged(const QList<QPair<int, int>> &ranges);
    void copyRequested();
private:
    QPointF mapToImagePos(const QPoint &viewPos) const;
    void setSelectionRanges(const QList<QPair<int, int>> &ranges, bool emitSignal = true);
    void updateCursorForImagePos(const QPointF &imagePos);

    QString m_currentPath;//当前图片路径
    QGraphicsPixmapItem *m_pixmapItem{nullptr};//当前图像的item
    bool m_isFitImage = false;//是否适应图片
    bool m_isFitWindow = false;//是否适应窗口
    qreal m_scal = 1.0;
    int   m_rotateAngel = 0; //旋转角度
    QImage *m_currentImage{nullptr};//当前原始图像
    QImage m_FilterImage{nullptr};//当前处理的图像
    QImage m_lightContrastImage{nullptr};//亮度曝光度图像

    OcrResult m_ocrResult;
    QList<QPair<int, int>> m_selRanges;     // V-2325 v4: user left selection (α90)
    QList<QPair<int, int>> m_focusRanges;   // V-2325 v4 D3: right→left linked focus (α55)
    QPointF m_dragAnchorPos;
    bool m_isTextSelecting = false;
    bool m_isPanning = false;
    QPoint m_lastPanPos;
    QMenu *m_contextMenu = nullptr;
};

#endif // IMAGEVIEW_H

// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "imageview.h"
#include "util/log.h"

#include <QPaintDevice>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QMessageBox>
#include <QThreadPool>
#include <qmath.h>
#include <QObject>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QClipboard>
#include <QApplication>
#include <QLineF>
#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

const qreal MAX_SCALE_FACTOR = 20.0;
const qreal MIN_SCALE_FACTOR = 0.029;
#define devicePixelRatioF  devicePixelRatio
ImageView::ImageView(QWidget *parent):
    QGraphicsView(parent)
{
    setMouseTracking(true);
    setDragMode(ScrollHandDrag);
    QGraphicsScene *scene = new QGraphicsScene(this);
    setScene(scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->grabGesture(Qt::PinchGesture);
    setAttribute(Qt::WA_AcceptTouchEvents);
    setFocusPolicy(Qt::StrongFocus);
    viewport()->setCursor(Qt::ArrowCursor);

    m_contextMenu = new QMenu(this);
    auto *selectAllAction = m_contextMenu->addAction(tr("Select All"));
    auto *copyAction = m_contextMenu->addAction(tr("Copy"));
    copyAction->setObjectName(QStringLiteral("ocrCopyAction"));
    connect(selectAllAction, &QAction::triggered, this, &ImageView::selectAllText);
    connect(copyAction, &QAction::triggered, this, &ImageView::copyRequested);
}

ImageView::~ImageView()
{
    if (m_currentImage) {
        delete m_currentImage;
    }
}

void ImageView::openImage(const QString &path)
{
    qCInfo(dmOcr) << "Opening image from path:" << path;
    if (scene()) {
        if (m_currentImage) {
            delete m_currentImage;
            m_currentImage = nullptr;
        }
        m_currentImage = new QImage(path);

        if (!m_currentImage->isNull()) {
            QPixmap pic = QPixmap::fromImage(*m_currentImage);
            qCDebug(dmOcr) << "Image loaded successfully, size:" << m_currentImage->size();

            scene()->clear();
            m_pixmapItem = new QGraphicsPixmapItem(pic);
            m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
            QRectF rect = m_pixmapItem->boundingRect();
            setSceneRect(rect);
            scene()->addItem(m_pixmapItem);
            fitWindow();
            m_currentPath = path;
        } else {
            qCWarning(dmOcr) << "Failed to load image from path:" << path;
        }
        m_FilterImage = image();
    }
}

void ImageView::openFilterImage(QImage img)
{
    qCInfo(dmOcr) << "Opening filtered image, size:" << img.size();
    clearOcrResult();
    if (!img.isNull() && scene()) {
        m_FilterImage = img;
    }
    QPixmap pic = QPixmap::fromImage(img);
    if (!pic.isNull()) {
        scene()->clear();
        m_pixmapItem = new QGraphicsPixmapItem(pic);
        m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
        QRectF rect = m_pixmapItem->boundingRect();
        setSceneRect(rect);
        scene()->addItem(m_pixmapItem);
        fitWindow();
    } else {
        qCWarning(dmOcr) << "Failed to convert filtered image to pixmap";
    }
}
qreal ImageView::windowRelativeScale() const
{
    //替换撑满方案
    QRectF bf = sceneRect();
    if (1.0 * width() / height() > 1.0 * bf.width() / bf.height()) {
        return 1.0 * height() / bf.height();
    } else {
        return 1.0 * width() / bf.width();
    }
}
void ImageView::fitWindow()
{
    qreal wrs = windowRelativeScale() * 0.95; //未知原因导致fitwindow后仍然可以小范围滑动图片，此处乘以0.95以抵消滑动
    m_scal = wrs;
    resetTransform();
    scale(wrs, wrs);

    m_isFitImage = false;
    m_isFitWindow = true;

}

void ImageView::fitImage()
{
    resetTransform();
    m_scal = 1.0;
    scale(1, 1);
    m_isFitImage = true;
    m_isFitWindow = false;
}

void ImageView::RotateImage(const int &index)
{
    if (!m_pixmapItem && scene()) {
        qCWarning(dmOcr) << "Cannot rotate: no image loaded";
        return;
    }
    qCInfo(dmOcr) << "Rotating image by" << index << "degrees";
    QPixmap pixmap = m_pixmapItem->pixmap();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QTransform rotate;
#else
    QMatrix rotate;
#endif
    rotate.rotate(index);

    pixmap = pixmap.transformed(rotate, Qt::FastTransformation);
    pixmap.setDevicePixelRatio(devicePixelRatioF());
    scene()->clear();
    resetTransform();
    m_pixmapItem = new QGraphicsPixmapItem(pixmap);
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    // Make sure item show in center of view after reload
    QRectF rect = m_pixmapItem->boundingRect();
    setSceneRect(rect);
    scene()->addItem(m_pixmapItem);

    autoFit();
    m_rotateAngel += index;
    qCDebug(dmOcr) << "Image rotation completed, total angle:" << m_rotateAngel;

    m_FilterImage = image();
}



//void ImageView::openImage(QImage *img)
//{
//    if (!img->isNull() && scene()) {
//        QPixmap pic = QPixmap::fromImage(*img);
//        if (!pic.isNull()) {
//            scene()->clear();
//            m_pixmapItem = new QGraphicsPixmapItem(pic);
//            m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
//            QRectF rect = m_pixmapItem->boundingRect();
//            setSceneRect(rect);
//            scene()->addItem(m_pixmapItem);
//            fitWindow();
//        }
//    }
//}

qreal ImageView::imageRelativeScale() const
{
    return transform().m11() / devicePixelRatioF();
}
void ImageView::autoFit()
{
    if (image().isNull()) {
        qCWarning(dmOcr) << "Cannot auto-fit: no image loaded";
        return;
    }

    QSize image_size = image().size();
    qCDebug(dmOcr) << "Auto-fitting image, size:" << image_size;

    if ((image_size.width() >= width() || image_size.height() >= height() - 150) && width() > 0 &&
            height() > 0) {
        fitWindow();
    } else {
        fitImage();
    }
}

void ImageView::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_isTextSelecting) {
        m_isTextSelecting = false;
        e->accept();
        return;
    }
    if (m_isPanning) {
        m_isPanning = false;
        e->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(e);
    updateCursorForImagePos(mapToImagePos(e->pos()));
}

void ImageView::mousePressEvent(QMouseEvent *e)
{
    // V-2325 v4 C4: Ctrl+left-drag pans the image; plain left-drag does manual
    // lasso text selection. (Reference used middle-button pan — overridden to
    // Ctrl+left so the common "drag to select text" gesture stays on left.)
    if (!m_ocrResult.isEmpty() && e->button() == Qt::LeftButton
        && (e->modifiers() & Qt::ControlModifier)) {
        m_isPanning = true;
        m_lastPanPos = e->pos();
        viewport()->setCursor(Qt::ClosedHandCursor);
        e->accept();
        return;
    }

    if (!m_ocrResult.isEmpty() && e->button() == Qt::LeftButton) {
        const QPointF imagePos = mapToImagePos(e->pos());
        m_dragAnchorPos = imagePos;
        m_isTextSelecting = true;

        const int charIndex = m_ocrResult.charIndexAt(imagePos);
        if (charIndex >= 0) {
            const auto &item = m_ocrResult.allChars.at(charIndex);
            setSelectionRanges({qMakePair(item.plainTextStart, item.plainTextEnd)});
        } else {
            setSelectionRanges({});
        }
        setFocus();
        e->accept();
        return;
    }

    if (m_ocrResult.isEmpty()) {
        QGraphicsView::mousePressEvent(e);
        viewport()->unsetCursor();
        viewport()->setCursor(Qt::ArrowCursor);
        return;
    }

    // Non-left buttons (middle/right) with OCR result present: forward to the
    // base class so its default behaviors (e.g. middle-button auto-scroll) keep
    // working instead of being swallowed.
    QGraphicsView::mousePressEvent(e);
}

void ImageView::mouseDoubleClickEvent(QMouseEvent *event)
{
    // V-2325 v4 D1: double-click selects ONLY the single clicked char (not the
    // whole line as in the reference). Use charIndexAt to find the char and
    // select exactly its [plainTextStart, plainTextEnd) range.
    if (!m_ocrResult.isEmpty() && event->button() == Qt::LeftButton) {
        const QPointF imagePos = mapToImagePos(event->pos());
        const int charIndex = m_ocrResult.charIndexAt(imagePos);
        if (charIndex >= 0) {
            const auto &item = m_ocrResult.allChars.at(charIndex);
            setSelectionRanges({qMakePair(item.plainTextStart, item.plainTextEnd)});
        } else {
            setSelectionRanges({});
        }
        event->accept();
        return;
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        const QPoint delta = event->pos() - m_lastPanPos;
        m_lastPanPos = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
        return;
    }

    if (m_isTextSelecting && (event->buttons() & Qt::LeftButton) && !m_ocrResult.isEmpty()) {
        const QPointF currentPos = mapToImagePos(event->pos());
        QRectF dragRect = QRectF(m_dragAnchorPos, currentPos).normalized();
        if (dragRect.width() < 2.0 && dragRect.height() < 2.0) {
            dragRect = QRectF(currentPos.x() - 1.0, currentPos.y() - 1.0, 2.0, 2.0);
        }
        setSelectionRanges(m_ocrResult.plainTextRangesForRect(dragRect));
        event->accept();
        return;
    }

    if (!m_ocrResult.isEmpty()) {
        updateCursorForImagePos(mapToImagePos(event->pos()));
        event->accept();
        return;
    }

    if (event->buttons() == Qt::NoButton) {
        viewport()->setCursor(Qt::ArrowCursor);
    } else {
        QGraphicsView::mouseMoveEvent(event);
        viewport()->setCursor(Qt::ClosedHandCursor);
    }
}

bool ImageView::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        return gestureEvent(static_cast<QGestureEvent *>(event));
    }
    return QGraphicsView::event(event);
}

bool ImageView::gestureEvent(QGestureEvent *event)
{
//    if (QGesture *swipe = event->gesture(Qt::SwipeGesture)) //滑动逻辑
//        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))//双指捏合
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}


const QImage ImageView::image()
{
    if (m_pixmapItem) {
        return m_pixmapItem->pixmap().toImage();
    } else {
        return QImage();
    }
}

void ImageView::resizeEvent(QResizeEvent *event)
{

    return QGraphicsView::resizeEvent(event);
}

void ImageView::wheelEvent(QWheelEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    qreal factor = qPow(1.2, event->angleDelta().y() / 240.0);
    scaleAtPoint(event->position().toPoint(), factor);
#else
    qreal factor = qPow(1.2, event->delta() / 240.0);
    scaleAtPoint(event->pos(), factor);
#endif

    event->accept();
}

void ImageView::handleGestureEvent(QGestureEvent *gesture)
{
    qDebug() << "------" << __FUNCTION__ << "";
    if (QGesture *pinch = gesture->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
}

void ImageView::pinchTriggered(QPinchGesture *gesture)
{
    qDebug() << "------" << __FUNCTION__ << "";
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    //缩放手势
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        QPoint pos = mapFromGlobal(gesture->centerPoint().toPoint());
        if (abs(gesture->scaleFactor() - 1) > 0.006) {
            qDebug() << "scaleFactor" << gesture->scaleFactor();
            scaleAtPoint(pos, gesture->scaleFactor());
        }
    }
}
void ImageView::scaleAtPoint(QPoint pos, qreal factor)
{
    // Remember zoom anchor point.
    const QPointF targetPos = pos;
    const QPointF targetScenePos = mapToScene(targetPos.toPoint());

    // Do the scaling.
    setScaleValue(factor);
    // Restore the zoom anchor point.
    //
    // The Basic idea here is we don't care how the scene is scaled or transformed,
    // we just want to restore the anchor point to the target position we've
    // remembered, in the coordinate of the view/viewport.
    const QPointF curPos = mapFromScene(targetScenePos);
    const QPointF centerPos = QPointF(width() / 2.0, height() / 2.0) + (curPos - targetPos);
    const QPointF centerScenePos = mapToScene(centerPos.toPoint());
    centerOn(static_cast<int>(centerScenePos.x()), static_cast<int>(centerScenePos.y()));
}
void ImageView::setScaleValue(qreal v)
{
    //由于矩阵被旋转，通过矩阵获取缩放因子，计算缩放比例错误，因此记录过程中的缩放因子来判断缩放比例
    m_scal *= v;
    qDebug() << m_scal;
    scale(v, v);
    //const qreal irs = imageRelativeScale() * devicePixelRatioF();
    // Rollback
    if (v < 1 && /*irs <= MIN_SCALE_FACTOR)*/m_scal < 0.03) {
        const qreal minv = MIN_SCALE_FACTOR / m_scal;
        // if (minv < 1.09) return;
        scale(minv, minv);
        m_scal *= minv;
    } else if (v > 1 && /*irs >= MAX_SCALE_FACTOR*/m_scal > 20) {
        const qreal maxv = MAX_SCALE_FACTOR / m_scal;
        scale(maxv, maxv);
        m_scal *= maxv;
    } else {
        m_isFitImage = false;
        m_isFitWindow = false;
    }

//    qreal rescale = imageRelativeScale() * devicePixelRatioF();
    //    if (rescale - 1 > -0.01 && rescale - 1 < 0.01) {
    //        emit checkAdaptImageBtn();
    //    } else {
    //        emit disCheckAdaptImageBtn();
    //    }
    emit scaled(m_scal * 100);
    emit showScaleLabel();

}

void ImageView::setOcrResult(const OcrResult &result)
{
    m_ocrResult = result;
    m_selRanges.clear();
    m_focusRanges.clear();
    setDragMode(QGraphicsView::NoDrag);
    viewport()->update();
}

void ImageView::clearOcrResult()
{
    m_ocrResult = {};
    m_selRanges.clear();
    m_focusRanges.clear();
    setDragMode(QGraphicsView::ScrollHandDrag);
    viewport()->update();
}

void ImageView::selectAllText()
{
    if (m_ocrResult.plainText.isEmpty()) {
        setSelectionRanges({});
        return;
    }
    setSelectionRanges({qMakePair(0, m_ocrResult.plainText.size())});
}

void ImageView::selectRange(int start, int end)
{
    // V-2325 v4 D3: right→left linkage sets the FOCUS tier (α55), distinct from
    // the user's own left selection (m_selRanges, α90). emitSignal=false to
    // avoid retriggering left→right (anti-loop via m_syncingSelection in
    // MainWidget).
    if (start < 0 || end <= start) {
        m_focusRanges.clear();
    } else {
        m_focusRanges = {qMakePair(start, end)};
    }
    viewport()->update();
}

bool ImageView::hasSelection() const
{
    return !m_selRanges.isEmpty();
}

QString ImageView::selectedText() const
{
    return m_ocrResult.textInRanges(m_selRanges);
}

QList<QPair<int, int>> ImageView::selectionRanges() const
{
    return m_selRanges;
}

QPointF ImageView::mapToImagePos(const QPoint &viewPos) const
{
    return mapToScene(viewPos);
}

void ImageView::setSelectionRanges(const QList<QPair<int, int>> &ranges, bool emitSignal)
{
    m_selRanges = ranges;
    viewport()->update();
    if (emitSignal) {
        emit selectionRangesChanged(ranges);
    }
}

void ImageView::updateCursorForImagePos(const QPointF &imagePos)
{
    if (m_ocrResult.isEmpty()) {
        viewport()->setCursor(Qt::ArrowCursor);
        return;
    }

    const int charIndex = m_ocrResult.charIndexAt(imagePos);
    viewport()->setCursor(charIndex >= 0 ? Qt::IBeamCursor : Qt::ArrowCursor);
}

void ImageView::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect)
    if (m_ocrResult.isEmpty()) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);  // V-2325 v4 D3: no outline (no border/seam)

    // V-2325 v4 D3: theme color QPalette::Highlight (overrides reference fixed
    // blue). Re-read on every paint so theme switches take effect immediately.
    const QColor highlight = DGuiApplicationHelper::instance()
        ->applicationPalette().color(QPalette::Highlight);

    // Focus tier (right→left linkage): α55, drawn first (lower z).
    if (!m_focusRanges.isEmpty()) {
        const auto focusAreas = m_ocrResult.highlightRectsForRanges(m_focusRanges);
        QColor focusColor = highlight;
        focusColor.setAlpha(55);
        painter->setBrush(focusColor);
        for (const QRectF &area : focusAreas) {
            if (!area.isEmpty()) {
                painter->drawRect(area);
            }
        }
    }

    // Selection tier (user left lasso/double-click/select-all): α90, on top.
    if (hasSelection()) {
        const auto highlightAreas = m_ocrResult.highlightRectsForRanges(m_selRanges);
        QColor selColor = highlight;
        selColor.setAlpha(90);
        painter->setBrush(selColor);
        for (const QRectF &area : highlightAreas) {
            if (!area.isEmpty()) {
                painter->drawRect(area);
            }
        }
    }

    painter->restore();
}

void ImageView::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_ocrResult.isEmpty() || m_contextMenu == nullptr) {
        QGraphicsView::contextMenuEvent(event);
        return;
    }

    for (QAction *action : m_contextMenu->actions()) {
        if (action->objectName() == QStringLiteral("ocrCopyAction")) {
            action->setEnabled(hasSelection());
        }
    }
    m_contextMenu->exec(event->globalPos());
}

void ImageView::keyPressEvent(QKeyEvent *event)
{
    if (!m_ocrResult.isEmpty() && event->matches(QKeySequence::SelectAll)) {
        selectAllText();
        event->accept();
        return;
    }

    if (!m_ocrResult.isEmpty() && event->matches(QKeySequence::Copy)) {
        if (hasSelection()) {
            emit copyRequested();
        }
        event->accept();
        return;
    }

    QGraphicsView::keyPressEvent(event);
}


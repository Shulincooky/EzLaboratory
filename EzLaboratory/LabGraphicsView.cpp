#include "LabGraphicsView.h"

LabGraphicsView::LabGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::NoDrag);
}

void LabGraphicsView::mousePressEvent(QMouseEvent* event)
{
    //// 1. 中键始终允许拖动画布
    //if (event->button() == Qt::MiddleButton) {
    //    m_isPanning = true;
    //    m_panButton = Qt::MiddleButton;
    //    setDragMode(QGraphicsView::ScrollHandDrag);

    //    QMouseEvent fakeEvent(
    //        QEvent::MouseButtonPress,
    //        event->position(),
    //        event->globalPosition(),
    //        Qt::LeftButton,
    //        Qt::LeftButton,
    //        event->modifiers()
    //    );
    //    QGraphicsView::mousePressEvent(&fakeEvent);
    //    return;
    //}

    // 2. 左键按到空白处：拖动画布
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->pos());

        if (!item) {
            m_isPanning = true;
            m_panButton = Qt::LeftButton;
            setDragMode(QGraphicsView::ScrollHandDrag);

            QMouseEvent fakeEvent(
                QEvent::MouseButtonPress,
                event->position(),
                event->globalPosition(),
                Qt::LeftButton,
                Qt::LeftButton,
                event->modifiers()
            );
            QGraphicsView::mousePressEvent(&fakeEvent);
            return;
        }
    }

    // 3. 左键按到物体上：保持物体拖拽逻辑
    QGraphicsView::mousePressEvent(event);
}

void LabGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_isPanning && event->button() == m_panButton) {
        QMouseEvent fakeEvent(
            QEvent::MouseButtonRelease,
            event->position(),
            event->globalPosition(),
            Qt::LeftButton,
            Qt::NoButton,
            event->modifiers()
        );
        QGraphicsView::mouseReleaseEvent(&fakeEvent);

        m_isPanning = false;
        m_panButton = Qt::NoButton;
        setDragMode(QGraphicsView::NoDrag);
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}
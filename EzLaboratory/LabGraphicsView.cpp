#include "LabGraphicsView.h"

LabGraphicsView::LabGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::NoDrag);
}

void LabGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = true;
        setDragMode(QGraphicsView::ScrollHandDrag);

        // 伪造一个左键按下事件，让 QGraphicsView 内建平移逻辑生效
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

    QGraphicsView::mousePressEvent(event);
}

void LabGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_isPanning && event->button() == Qt::MiddleButton) {
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
        setDragMode(QGraphicsView::NoDrag);
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}
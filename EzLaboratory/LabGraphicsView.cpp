#include "LabGraphicsView.h"
#include "BeakerItem.h"
#include "LabItem.h"

#include <QMimeData>
#include <QGraphicsScene>
namespace
{
    constexpr const char* kLabwareMimeType = "application/x-ezlaboratory-labware";
}

LabGraphicsView::LabGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::NoDrag);
    setAcceptDrops(true);
}

void LabGraphicsView::mousePressEvent(QMouseEvent* event)
{
    // 左键按到空白处：拖动画布
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->pos());

        if (!item) {
            m_isPanning = true;
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

    // 左键按到物体上：保持物体拖拽逻辑
    QGraphicsView::mousePressEvent(event);
}

void LabGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_isPanning && event->button() == Qt::LeftButton) {
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

void LabGraphicsView::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, QColor(241, 241, 241));
}

void LabGraphicsView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(kLabwareMimeType)) {
        event->acceptProposedAction();
        return;
    }

    QGraphicsView::dragEnterEvent(event);
}

void LabGraphicsView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat(kLabwareMimeType)) {
        event->acceptProposedAction();
        return;
    }

    QGraphicsView::dragMoveEvent(event);
}

void LabGraphicsView::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasFormat(kLabwareMimeType)) {
        QGraphicsView::dropEvent(event);
        return;
    }

    if (!scene()) {
        event->ignore();
        return;
    }

    const QByteArray data = event->mimeData()->data(kLabwareMimeType);
    const QList<QByteArray> parts = data.split('|');
    if (parts.size() != 2) {
        event->ignore();
        return;
    }

    const QString type = QString::fromUtf8(parts[0]);
    const int limit = parts[1].toInt();

    int currentCount = 0;
    const auto itemsInScene = scene()->items();
    for (QGraphicsItem* item : itemsInScene) {
        auto* labItem = dynamic_cast<LabItem*>(item);
        if (!labItem)
            continue;

        if (labItem->itemName().compare(type, Qt::CaseInsensitive) == 0 ||
            (type == "beaker" && labItem->itemName() == "Beaker")) {
            ++currentCount;
        }
    }

    if (limit > 0 && currentCount >= limit) {
        event->ignore();
        return;
    }

    LabItem* newItem = nullptr;

    if (type == "beaker") {
        newItem = new BeakerItem();
    }

    if (!newItem) {
        event->ignore();
        return;
    }

    QPointF scenePos = mapToScene(event->position().toPoint());
    newItem->setPos(scenePos);
    scene()->addItem(newItem);

    event->acceptProposedAction();
}
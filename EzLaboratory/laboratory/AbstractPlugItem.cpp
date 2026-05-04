#include "AbstractPlugItem.h"
#include "AbstractBottleItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <limits>

AbstractPlugItem::AbstractPlugItem(const QString& itemType,
    const QString& itemName,
    const QString& imagePath,
    const QSizeF& itemSize,
    QGraphicsItem* parent)
    : LabItem(itemType, itemName, imagePath, itemSize, parent)
{
    setZValue(attachedZValue());
    setTransformOriginPoint(boundingRect().center());
}

void AbstractPlugItem::attachToBottle(AbstractBottleItem* bottle)
{
    if (!bottle) {
        return;
    }

    if (m_attachedBottle == bottle && parentItem() == bottle) {
        return;
    }

    if (bottle->hasPlug() && bottle->plug() != this) {
        return;
    }

    if (m_attachedBottle) {
        m_attachedBottle->clearPlugReference(this);
    }

    m_attachedBottle = bottle;
    m_attachedBottle->setPlugReference(this);

    setParentItem(bottle);
    setRotation(0.0);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setZValue(attachedZValue());

    const QPointF localPos = bottle->plugLocalTopLeft(this);
    setPos(localPos);
}

void AbstractPlugItem::detachFromBottle()
{
    if (!scene()) {
        return;
    }

    const QPointF scenePosBefore = mapToScene(QPointF(0, 0));

    if (m_attachedBottle) {
        m_attachedBottle->clearPlugReference(this);
        m_attachedBottle = nullptr;
    }

    setParentItem(nullptr);
    setPos(scenePosBefore);
    setRotation(detachedRotation());
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setZValue(detachedZValue());
}

bool AbstractPlugItem::isAttached() const
{
    return m_attachedBottle != nullptr;
}

AbstractBottleItem* AbstractPlugItem::attachedBottle() const
{
    return m_attachedBottle;
}

QPointF AbstractPlugItem::plugTipScenePos() const
{
    return mapToScene(plugTipLocalPos());
}

void AbstractPlugItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isAttached()) {
        detachFromBottle();
    }

    LabItem::mousePressEvent(event);
}

void AbstractPlugItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    LabItem::mouseReleaseEvent(event);

    if (event->button() != Qt::LeftButton) {
        return;
    }

    AbstractBottleItem* targetBottle = findBestSnapBottle();
    if (targetBottle) {
        attachToBottle(targetBottle);
    }
}

qreal AbstractPlugItem::attachedZValue() const
{
    return 10.0;
}

qreal AbstractPlugItem::detachedZValue() const
{
    return 100.0;
}

qreal AbstractPlugItem::detachedRotation() const
{
    return 180.0;
}

AbstractBottleItem* AbstractPlugItem::findBestSnapBottle() const
{
    if (!scene()) {
        return nullptr;
    }

    const QPointF tipPos = plugTipScenePos();

    AbstractBottleItem* bestBottle = nullptr;
    qreal bestDistance = std::numeric_limits<qreal>::max();

    const QList<QGraphicsItem*> allItems = scene()->items();
    for (QGraphicsItem* item : allItems) {
        auto* bottle = dynamic_cast<AbstractBottleItem*>(item);
        if (!bottle) {
            continue;
        }

        if (bottle->hasPlug() && bottle->plug() != this) {
            continue;
        }

        const qreal dist = QLineF(tipPos, bottle->mouthScenePos()).length();
        if (dist <= bottle->snapRadius() && dist < bestDistance) {
            bestDistance = dist;
            bestBottle = bottle;
        }
    }

    return bestBottle;
}
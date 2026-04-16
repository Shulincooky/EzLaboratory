#include "BottleStopperItem.h"
#include "NarrowBottleItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QtMath>

namespace
{
    // 这里先写成一个明确常量，后面你只要把路径替换成你 generic 里真实的瓶塞图片即可
    constexpr const char* kStopperImagePath =
        ":/EzLaboratory/resources/image/glassware/generic/narrow-mouth_bottle/plug.png";
}

BottleStopperItem::BottleStopperItem(QGraphicsItem* parent)
    : LabItem(
        "narrow_bottle_stopper",
        "NarrowBottleStopper",
        kStopperImagePath,
        QSizeF(42, 56),
        parent)
{
    setZValue(10.0);
    setTransformOriginPoint(boundingRect().center());
}

void BottleStopperItem::attachToBottle(NarrowBottleItem* bottle)
{
    if (!bottle) {
        return;
    }

    if (m_attachedBottle == bottle && parentItem() == bottle) {
        return;
    }
    // 安全保护：已经有别的塞子占用时，不允许挂上去
    if (bottle->hasStopper() && bottle->stopper() != this) {
        return;
    }

    if (m_attachedBottle) {
        m_attachedBottle->clearStopperReference(this);
    }

    m_attachedBottle = bottle;
    m_attachedBottle->setStopperReference(this);

    setParentItem(bottle);
    setRotation(0.0);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    const QPointF localPos = bottle->stopperLocalTopLeft(this);
    setPos(localPos);
}

void BottleStopperItem::detachFromBottle()
{
    if (!scene()) {
        return;
    }

    QPointF scenePosBefore = mapToScene(QPointF(0, 0));

    if (m_attachedBottle) {
        m_attachedBottle->clearStopperReference(this);
        m_attachedBottle = nullptr;
    }

    setParentItem(nullptr);
    setPos(scenePosBefore);
    setRotation(180.0);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setZValue(10.0);
}

bool BottleStopperItem::isAttached() const
{
    return m_attachedBottle != nullptr;
}

NarrowBottleItem* BottleStopperItem::attachedBottle() const
{
    return m_attachedBottle;
}

QPointF BottleStopperItem::plugTipScenePos() const
{
    return mapToScene(QPointF(boundingRect().width() * 0.5, boundingRect().height()));
}

void BottleStopperItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isAttached()) {
        detachFromBottle();
    }

    LabItem::mousePressEvent(event);
}

void BottleStopperItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    LabItem::mouseReleaseEvent(event);

    if (event->button() != Qt::LeftButton) {
        return;
    }

    NarrowBottleItem* targetBottle = findBestSnapBottle();
    if (targetBottle) {
        attachToBottle(targetBottle);
    }
}

NarrowBottleItem* BottleStopperItem::findBestSnapBottle() const
{
    if (!scene()) {
        return nullptr;
    }

    const QPointF tipPos = plugTipScenePos();

    NarrowBottleItem* bestBottle = nullptr;
    qreal bestDistance = std::numeric_limits<qreal>::max();

    const QList<QGraphicsItem*> allItems = scene()->items();
    for (QGraphicsItem* item : allItems) {
        auto* bottle = dynamic_cast<NarrowBottleItem*>(item);
        if (!bottle) {
            continue;
        }

        // 只允许吸附到空瓶口，或者吸回自己原本所在的那个瓶子
        if (bottle->hasStopper() && bottle->stopper() != this) {
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
#include "NarrowBottleItem.h"
#include "BottleStopperItem.h"

namespace
{
    // 这里也先写成固定资源路径，你本地把文件名对上即可
    constexpr const char* kBottleBodyImagePath =
        ":/EzLaboratory/resources/image/glassware/generic/narrow-mouth_bottle/bottle.png";
}

NarrowBottleItem::NarrowBottleItem(QGraphicsItem* parent)
    : LabItem(
        "narrow_bottle",
        "NarrowBottle",
        kBottleBodyImagePath,
        QSizeF(96, 180),
        parent)
{
    m_stopper = new BottleStopperItem(this);
    m_stopper->attachToBottle(this);
}

NarrowBottleItem::~NarrowBottleItem()
{
}

QPointF NarrowBottleItem::mouthLocalPos() const
{
    // 这里按当前 96x180 的瓶子图先给一个合理口部锚点
    // 以后你只要看贴图微调这两个数值即可
    return QPointF(48.0, 20.0);
}

QPointF NarrowBottleItem::mouthScenePos() const
{
    return mapToScene(mouthLocalPos());
}

QPointF NarrowBottleItem::stopperLocalTopLeft(const BottleStopperItem* stopper) const
{
    if (!stopper) {
        return QPointF();
    }

    const QRectF stopperRect = stopper->boundingRect();
    const QPointF mouth = mouthLocalPos();

    return QPointF(
        mouth.x() - stopperRect.width() * 0.5,
        mouth.y() - stopperRect.height() + 27.0
    );
}

qreal NarrowBottleItem::snapRadius() const
{
    return 36.0;
}

void NarrowBottleItem::setStopperReference(BottleStopperItem* stopper)
{
    m_stopper = stopper;
}

void NarrowBottleItem::clearStopperReference(BottleStopperItem* stopper)
{
    if (m_stopper == stopper) {
        m_stopper = nullptr;
    }
}

bool NarrowBottleItem::hasStopper() const
{
    return m_stopper != nullptr;
}

BottleStopperItem* NarrowBottleItem::stopper() const
{
    return m_stopper;
}
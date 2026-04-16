#include "NarrowBottleItem.h"
#include "NarrowBottlePlugItem.h"

namespace
{
    constexpr const char* kBottleBodyImagePath =
        ":/EzLaboratory/resources/image/glassware/generic/narrow-mouth_bottle/bottle.png";
}

NarrowBottleItem::NarrowBottleItem(QGraphicsItem* parent)
    : AbstractBottleItem(
        "narrow_bottle",
        "NarrowBottle",
        kBottleBodyImagePath,
        QSizeF(96, 180),
        parent)
{
    initializePlug(new NarrowBottlePlugItem(this));
}

NarrowBottleItem::~NarrowBottleItem()
{
}

QPointF NarrowBottleItem::mouthLocalPos() const
{
    return QPointF(48.0, 18.0);
}

qreal NarrowBottleItem::snapRadius() const
{
    return 36.0;
}

qreal NarrowBottleItem::plugInsertOffsetY() const
{
    return 27.0;
}
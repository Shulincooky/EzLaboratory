#include "NarrowBottleItem.h"
#include "NarrowBottlePlugItem.h"
#include "BottleLabelItem.h"

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
    initializeLabel(new BottleLabelItem(this));
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

QPointF NarrowBottleItem::labelLocalPos() const
{
    return QPointF(25.0, 78.0);
}

QSizeF NarrowBottleItem::labelLogicalSize() const
{
    return QSizeF(46.0, 34.0);
}
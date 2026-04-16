#include "WideBottleItem.h"
#include "WideBottlePlugItem.h"
#include "BottleLabelItem.h"

namespace
{
    constexpr const char* kBottleBodyImagePath =
        ":/EzLaboratory/resources/image/glassware/generic/wide-mouth_bottle/bottle.png";
}

WideBottleItem::WideBottleItem(QGraphicsItem* parent)
    : AbstractBottleItem(
        "wide_bottle",
        "WideBottle",
        kBottleBodyImagePath,
        QSizeF(108, 180),
        parent)
{
    initializePlug(new WideBottlePlugItem(this));
    initializeLabel(new BottleLabelItem(this));
}

WideBottleItem::~WideBottleItem()
{
}

QPointF WideBottleItem::mouthLocalPos() const
{
    return QPointF(54.0, 20.0);
}

qreal WideBottleItem::snapRadius() const
{
    return 42.0;
}

qreal WideBottleItem::plugInsertOffsetY() const
{
    return 24.0;
}

QPointF WideBottleItem::labelLocalPos() const
{
    return QPointF(20.0, 76.0);
}

QSizeF WideBottleItem::labelLogicalSize() const
{
    return QSizeF(60.0, 38.0);
}
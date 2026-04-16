#include "WideBottlePlugItem.h"

namespace
{
    constexpr const char* kPlugImagePath =
        ":/EzLaboratory/resources/image/glassware/generic/wide-mouth_bottle/plug.png";
}

WideBottlePlugItem::WideBottlePlugItem(QGraphicsItem* parent)
    : AbstractPlugItem(
        "wide_bottle_plug",
        "WideBottlePlug",
        kPlugImagePath,
        QSizeF(54, 48),
        parent)
{
}

QPointF WideBottlePlugItem::plugTipLocalPos() const
{
    return QPointF(boundingRect().width() * 0.5, boundingRect().height() * 0.72);
}
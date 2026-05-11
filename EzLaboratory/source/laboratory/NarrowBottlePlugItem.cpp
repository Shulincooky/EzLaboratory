#include "NarrowBottlePlugItem.h"

namespace
{
    constexpr const char* kPlugImagePath =
        ":/EzLaboratory/images/labcontainer/narrow-neck_bottle/plug.png";
}

NarrowBottlePlugItem::NarrowBottlePlugItem(QGraphicsItem* parent)
    : AbstractPlugItem(
        "narrow_bottle_plug",
        "NarrowBottlePlug",
        kPlugImagePath,
        QSizeF(42, 56),
        parent)
{
}

QPointF NarrowBottlePlugItem::plugTipLocalPos() const
{
    return QPointF(boundingRect().width() * 0.5, boundingRect().height() * 0.68);
}

#include "BeakerItem.h"

BeakerItem::BeakerItem(QGraphicsItem* parent)
    : LabItem(
        "beaker",
        "Beaker",
        ":/EzLaboratory/resources/image/glassware/beaker.png",
        QSizeF(100, 140),
        parent)
{
}
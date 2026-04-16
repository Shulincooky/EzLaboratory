#include "BeakerItem.h"

BeakerItem::BeakerItem(QGraphicsItem* parent)
    : LabItem(
        "beaker",
        "Beaker",
        ":/EzLaboratory/resources/image/glassware/beaker.png",
        QSizeF(100*1.5, 140*1.5),
        parent)
{
}
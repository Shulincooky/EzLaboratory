#include "BeakerItem.h"
#include "LiquidItem.h"

BeakerItem::BeakerItem(QGraphicsItem* parent)
    : AbstractLiquidContainerItem(
        "beaker",
        "Beaker",
        ":/EzLaboratory/resources/image/glassware/beaker.png",
        QSizeF(100 * 1.5, 140 * 1.5),
        parent)
{
    initializeLiquid(new LiquidItem(this));
}

QRectF BeakerItem::liquidRectLocal() const
{
    return QRectF(18.0, 36.0, itemSize().width() - 36.0, itemSize().height() - 52.0);
}

QColor BeakerItem::defaultLiquidColor() const
{
    return QColor(90, 150, 255, 135);
}

qreal BeakerItem::defaultLiquidFillRatio() const
{
    return 0.50;
}
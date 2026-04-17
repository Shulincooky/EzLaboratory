#include "BeakerItem.h"
#include "LiquidItem.h"

#include <QPainterPath>

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
QPainterPath BeakerItem::liquidClipPathLocal() const
{
    const QRectF r = liquidRectLocal();

    QPainterPath path;

    // 上口略宽、下部略窄一点，模拟烧杯内腔
    const qreal insetTop = 4.0;
    const qreal insetBottom = 4.0;

    path.moveTo(r.left() + insetTop, r.top());
    path.lineTo(r.right() - insetTop, r.top());
    path.lineTo(r.right() - insetBottom, r.bottom());
    path.lineTo(r.left() + insetBottom, r.bottom());
    path.closeSubpath();

    return path;
}
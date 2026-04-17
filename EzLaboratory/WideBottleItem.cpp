#include "WideBottleItem.h"
#include "WideBottlePlugItem.h"
#include "BottleLabelItem.h"
#include "LiquidItem.h"

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
    setLiquidRenderingEnabled(false);
}

WideBottleItem::~WideBottleItem()
{
}

WideBottleItem* WideBottleItem::createInstance(const BottleLabelData& data,
    bool enableLiquid,
    const QColor& liquidColor,
    QGraphicsItem* parent)
{
    auto* bottle = new WideBottleItem(parent);
    bottle->finalizeInstance(data);

    bottle->setLiquidRenderingEnabled(enableLiquid);
    if (enableLiquid && liquidColor.isValid()) {
        bottle->setLiquidColor(liquidColor);
    }

    return bottle;
}

QPointF WideBottleItem::mouthLocalPos() const
{
    return QPointF(54.0, 15.0);
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
QRectF WideBottleItem::liquidRectLocal() const
{
    const qreal leftInset = 10.0;
    const qreal rightInset = 10.0;
    const qreal topInset = 80.0;
    const qreal bottomInset = 14.0;

    return QRectF(
        leftInset,
        topInset,
        itemSize().width() - leftInset - rightInset,
        itemSize().height() - topInset - bottomInset
    );
}

QColor WideBottleItem::defaultLiquidColor() const
{
    return QColor(90, 150, 255, 125);
}

qreal WideBottleItem::defaultLiquidFillRatio() const
{
    return 1.0;
}
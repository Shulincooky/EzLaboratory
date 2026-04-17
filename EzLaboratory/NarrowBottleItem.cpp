#include "NarrowBottleItem.h"
#include "NarrowBottlePlugItem.h"
#include "BottleLabelItem.h"
#include "LiquidItem.h"

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
    setLiquidRenderingEnabled(false);
}

NarrowBottleItem::~NarrowBottleItem()
{
}

NarrowBottleItem* NarrowBottleItem::createInstance(const BottleLabelData& data,
    bool enableLiquid,
    const QColor& liquidColor,
    QGraphicsItem* parent)
{
    auto* bottle = new NarrowBottleItem(parent);
    bottle->finalizeInstance(data);

    bottle->setLiquidRenderingEnabled(enableLiquid);
    if (enableLiquid && liquidColor.isValid()) {
        bottle->setLiquidColor(liquidColor);
    }

    return bottle;
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
QRectF NarrowBottleItem::liquidRectLocal() const
{
    // 只覆盖瓶腹到瓶颈以下，默认 fillRatio=1.0 时就是“满到瓶颈”
    return QRectF(24.0, 48.0, itemSize().width() - 48.0, itemSize().height() - 62.0);
}

QColor NarrowBottleItem::defaultLiquidColor() const
{
    return QColor(90, 150, 255, 125);
}

qreal NarrowBottleItem::defaultLiquidFillRatio() const
{
    return 1.0;
}
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

QColor NarrowBottleItem::defaultLiquidColor() const
{
    return QColor(90, 150, 255, 125);
}

qreal NarrowBottleItem::defaultLiquidFillRatio() const
{
    return 1.0;
}
QPointF NarrowBottleItem::pourPivotLocalPos() const
{
    // 近似取“最上方最左侧瓶口接触点”
    return QPointF(mouthLocalPos().x() - 8.0, mouthLocalPos().y());
}
#pragma once

#include "AbstractBottleItem.h"
#include <QColor>

class NarrowBottleItem : public AbstractBottleItem
{
    Q_OBJECT

public:
    explicit NarrowBottleItem(QGraphicsItem* parent = nullptr);
    ~NarrowBottleItem() override;

    static NarrowBottleItem* createInstance(const BottleLabelData& data,
        bool enableLiquid = false,
        const QColor& liquidColor = QColor(),
        QGraphicsItem* parent = nullptr);

    QPointF mouthLocalPos() const override;
    qreal snapRadius() const override;
    QPointF pourPivotLocalPos() const override;

protected:
    qreal plugInsertOffsetY() const override;
    QPointF labelLocalPos() const override;
    QSizeF labelLogicalSize() const override;

    QRectF liquidRectLocal() const override;
    QColor defaultLiquidColor() const override;
    qreal defaultLiquidFillRatio() const override;
};
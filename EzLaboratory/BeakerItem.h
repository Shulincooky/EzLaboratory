#pragma once
#include "AbstractLiquidContainerItem.h"

class BeakerItem : public AbstractLiquidContainerItem
{
    Q_OBJECT

public:
    explicit BeakerItem(QGraphicsItem* parent = nullptr);

protected:
    QRectF liquidRectLocal() const override;
    QColor defaultLiquidColor() const override;
    qreal defaultLiquidFillRatio() const override;
    QPainterPath liquidClipPathLocal() const override;
};
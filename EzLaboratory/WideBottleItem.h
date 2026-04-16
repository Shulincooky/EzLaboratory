#pragma once

#include "AbstractBottleItem.h"

class WideBottleItem : public AbstractBottleItem
{
    Q_OBJECT

public:
    explicit WideBottleItem(QGraphicsItem* parent = nullptr);
    ~WideBottleItem() override;
    static WideBottleItem* createInstance(const BottleLabelData& data,
        QGraphicsItem* parent = nullptr);

    QPointF mouthLocalPos() const override;
    qreal snapRadius() const override;

protected:
    qreal plugInsertOffsetY() const override;
    QPointF labelLocalPos() const override;
    QSizeF labelLogicalSize() const override;
};
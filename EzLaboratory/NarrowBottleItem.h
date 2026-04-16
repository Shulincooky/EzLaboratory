#pragma once

#include "AbstractBottleItem.h"

class NarrowBottleItem : public AbstractBottleItem
{
    Q_OBJECT

public:
    explicit NarrowBottleItem(QGraphicsItem* parent = nullptr);
    ~NarrowBottleItem() override;

    QPointF mouthLocalPos() const override;
    qreal snapRadius() const override;

protected:
    qreal plugInsertOffsetY() const override;
    QPointF labelLocalPos() const override;
    QSizeF labelLogicalSize() const override;
};
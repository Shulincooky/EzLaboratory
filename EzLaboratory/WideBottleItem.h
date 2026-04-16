#pragma once

#include "AbstractBottleItem.h"

class WideBottleItem : public AbstractBottleItem
{
    Q_OBJECT

public:
    explicit WideBottleItem(QGraphicsItem* parent = nullptr);
    ~WideBottleItem() override;

    QPointF mouthLocalPos() const override;
    qreal snapRadius() const override;

protected:
    qreal plugInsertOffsetY() const override;
};
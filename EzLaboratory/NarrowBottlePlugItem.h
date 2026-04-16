#pragma once

#include "AbstractPlugItem.h"

class NarrowBottlePlugItem : public AbstractPlugItem
{
    Q_OBJECT

public:
    explicit NarrowBottlePlugItem(QGraphicsItem* parent = nullptr);

protected:
    QPointF plugTipLocalPos() const override;
};
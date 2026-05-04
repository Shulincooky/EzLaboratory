#pragma once

#include "AbstractPlugItem.h"

class WideBottlePlugItem : public AbstractPlugItem
{
    Q_OBJECT

public:
    explicit WideBottlePlugItem(QGraphicsItem* parent = nullptr);

protected:
    QPointF plugTipLocalPos() const override;
};
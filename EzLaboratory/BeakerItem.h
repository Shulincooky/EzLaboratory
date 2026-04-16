#pragma once

#include "LabItem.h"

class BeakerItem : public LabItem
{
    Q_OBJECT

public:
    explicit BeakerItem(QGraphicsItem* parent = nullptr);
};
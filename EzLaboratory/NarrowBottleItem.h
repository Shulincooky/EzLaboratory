#pragma once

#include "LabItem.h"

class BottleStopperItem;

class NarrowBottleItem : public LabItem
{
    Q_OBJECT

public:
    explicit NarrowBottleItem(QGraphicsItem* parent = nullptr);
    ~NarrowBottleItem() override;

    QPointF mouthLocalPos() const;
    QPointF mouthScenePos() const;

    QPointF stopperLocalTopLeft(const BottleStopperItem* stopper) const;
    qreal snapRadius() const;

    void setStopperReference(BottleStopperItem* stopper);
    void clearStopperReference(BottleStopperItem* stopper);

private:
    BottleStopperItem* m_stopper = nullptr;
};
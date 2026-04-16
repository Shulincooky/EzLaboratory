#pragma once

#include "LabItem.h"

class NarrowBottleItem;

class BottleStopperItem : public LabItem
{
    Q_OBJECT

public:
    explicit BottleStopperItem(QGraphicsItem* parent = nullptr);

    void attachToBottle(NarrowBottleItem* bottle);
    void detachFromBottle();

    bool isAttached() const;
    NarrowBottleItem* attachedBottle() const;

    QPointF plugTipScenePos() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    NarrowBottleItem* m_attachedBottle = nullptr;

    NarrowBottleItem* findBestSnapBottle() const;
};
#pragma once

#include "LabItem.h"

class AbstractBottleItem;
class QGraphicsSceneMouseEvent;

class AbstractPlugItem : public LabItem
{
    Q_OBJECT

public:
    explicit AbstractPlugItem(const QString& itemType,
        const QString& itemName,
        const QString& imagePath,
        const QSizeF& itemSize,
        QGraphicsItem* parent = nullptr);

    void attachToBottle(AbstractBottleItem* bottle);
    void detachFromBottle();

    bool isAttached() const;
    AbstractBottleItem* attachedBottle() const;

    QPointF plugTipScenePos() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    virtual QPointF plugTipLocalPos() const = 0;
    virtual qreal attachedZValue() const;
    virtual qreal detachedZValue() const;
    virtual qreal detachedRotation() const;

private:
    AbstractBottleItem* m_attachedBottle = nullptr;

    AbstractBottleItem* findBestSnapBottle() const;
};
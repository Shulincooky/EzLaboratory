#pragma once

#include "LabItem.h"

class AbstractPlugItem;

class AbstractBottleItem : public LabItem
{
    Q_OBJECT

public:
    explicit AbstractBottleItem(const QString& itemType,
        const QString& itemName,
        const QString& imagePath,
        const QSizeF& itemSize,
        QGraphicsItem* parent = nullptr);
    ~AbstractBottleItem() override;

    virtual QPointF mouthLocalPos() const = 0;
    QPointF mouthScenePos() const;

    QPointF plugLocalTopLeft(const AbstractPlugItem* plug) const;
    virtual qreal snapRadius() const = 0;

    void setPlugReference(AbstractPlugItem* plug);
    void clearPlugReference(AbstractPlugItem* plug);

    bool hasPlug() const;
    AbstractPlugItem* plug() const;

protected:
    void initializePlug(AbstractPlugItem* plug);
    virtual qreal plugInsertOffsetY() const = 0;
    virtual qreal plugOffsetX() const;

private:
    AbstractPlugItem* m_plug = nullptr;
};
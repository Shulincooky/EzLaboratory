#pragma once

#include "LabItem.h"
#include "BottleLabelData.h"

class AbstractPlugItem;
class BottleLabelItem;

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

    void setLabelData(const BottleLabelData& data);
    BottleLabelData labelData() const;

protected:
    void initializePlug(AbstractPlugItem* plug);
    void initializeLabel(BottleLabelItem* label);

    virtual qreal plugInsertOffsetY() const = 0;
    virtual qreal plugOffsetX() const;

    virtual QPointF labelLocalPos() const = 0;
    virtual QSizeF labelLogicalSize() const = 0;
    virtual QString labelTemplatePath() const;

private:
    AbstractPlugItem* m_plug = nullptr;
    BottleLabelItem* m_labelItem = nullptr;
    BottleLabelData m_labelData;
};
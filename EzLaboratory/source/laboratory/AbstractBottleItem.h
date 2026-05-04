#pragma once

#include "AbstractLiquidContainerItem.h"
#include "BottleLabelData.h"

class AbstractPlugItem;
class BottleLabelItem;

class AbstractBottleItem : public AbstractLiquidContainerItem
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
    void setLabelOffset(const QPointF& offset);
    QPointF labelOffset() const;
    virtual QPointF pourPivotLocalPos() const;

protected:
    void initializePlug(AbstractPlugItem* plug);
    void initializeLabel(BottleLabelItem* label);
    void finalizeInstance(const BottleLabelData& data);

    virtual qreal plugInsertOffsetY() const = 0;
    virtual qreal plugOffsetX() const;

    virtual QRectF liquidRectLocal() const = 0;
    virtual QColor defaultLiquidColor() const override;
    virtual qreal defaultLiquidFillRatio() const override;
    virtual QPainterPath liquidClipPathLocal() const override;

    virtual QPointF labelLocalPos() const = 0;
    virtual QSizeF labelLogicalSize() const = 0;
    virtual QString labelTemplatePath() const;

private:
    AbstractPlugItem* m_plug = nullptr;
    BottleLabelItem* m_labelItem = nullptr;
    BottleLabelData m_labelData;
    QPointF m_labelOffset;
};
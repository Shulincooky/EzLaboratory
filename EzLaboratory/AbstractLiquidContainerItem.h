#pragma once

#include "LabItem.h"

class LiquidItem;
class QVariantAnimation;

class AbstractLiquidContainerItem : public LabItem
{
    Q_OBJECT

public:
    explicit AbstractLiquidContainerItem(const QString& itemType,
        const QString& itemName,
        const QString& imagePath,
        const QSizeF& itemSize,
        QGraphicsItem* parent = nullptr);
    ~AbstractLiquidContainerItem() override;

    void setLiquidColor(const QColor& color);
    void startLiquidColorTransition(const QColor& targetColor, int durationMs = 2000);
    void setLiquidFillRatio(qreal ratio);

    void startContainerSway(qreal maxAngleDeg = 10.0, int durationMs = 1600);

protected:
    void initializeLiquid(LiquidItem* liquid);
    void refreshLiquidGeometry();

    virtual QRectF liquidRectLocal() const = 0;
    virtual QColor defaultLiquidColor() const;
    virtual qreal defaultLiquidFillRatio() const;

private:
    QPainterPath buildAlphaClipPath() const;

private:
    LiquidItem* m_liquidItem = nullptr;
    QVariantAnimation* m_swayAnimation = nullptr;
};
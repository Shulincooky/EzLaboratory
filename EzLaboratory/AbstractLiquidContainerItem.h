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

    void setLiquidLevel(qreal level);
    QColor liquidColor() const;
    qreal liquidLevel() const;

    void setLiquidRenderingEnabled(bool enabled);
    bool liquidRenderingEnabled() const;
    bool hasLiquidItem() const;

    void startContainerSway(qreal maxAngleDeg = 10.0, int durationMs = 1600);

protected:
    void initializeLiquid(LiquidItem* liquid);
    void refreshLiquidGeometry();

    void ensureLiquidCreated();
    void destroyLiquid();

    virtual LiquidItem* createLiquidItem();


    virtual QRectF liquidRectLocal() const = 0;
    virtual QColor defaultLiquidColor() const;
    virtual qreal defaultLiquidFillRatio() const;
    virtual QPainterPath liquidClipPathLocal() const;

private:
    LiquidItem* m_liquidItem = nullptr;
    QVariantAnimation* m_swayAnimation = nullptr;

    bool m_liquidRenderingEnabled = false;
    QColor m_storedLiquidColor;
    qreal m_storedLiquidLevel = 0.0;
};
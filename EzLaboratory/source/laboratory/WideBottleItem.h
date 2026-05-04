#pragma once

#include "AbstractBottleItem.h"
#include "SolidScatterItem.h"
#include <QColor>

class WideBottleItem : public AbstractBottleItem
{
    Q_OBJECT

public:
    explicit WideBottleItem(QGraphicsItem* parent = nullptr);
    ~WideBottleItem() override;

    static WideBottleItem* createInstance(const BottleLabelData& data,
        bool enableLiquid = false,
        const QColor& liquidColor = QColor(),
        bool enableSolid = false,
        const QString& solidTexturePath = QString(),
        qreal solidFillRatio = 0.0,
        QGraphicsItem* parent = nullptr);

    QPointF mouthLocalPos() const override;
    qreal snapRadius() const override;

    void setSolidTexturePath(const QString& texturePath);
    QString solidTexturePath() const;

    void setSolidFillRatio(qreal ratio);
    qreal solidFillRatio() const;

    void setSolidRenderingEnabled(bool enabled);
    bool solidRenderingEnabled() const;
    bool hasSolidItem() const;

    void setSolidChemicalId(const QString& chemicalId);
    QString solidChemicalId() const;
    bool takeSolidForTweezers(QString* chemicalId, QString* texturePath);

protected:
    qreal plugInsertOffsetY() const override;
    QPointF labelLocalPos() const override;
    QSizeF labelLogicalSize() const override;
    QRectF liquidRectLocal() const override;
    QColor defaultLiquidColor() const override;
    qreal defaultLiquidFillRatio() const override;

private:
    void ensureSolidCreated();
    void destroySolid();
    void refreshSolidGeometry();

private:
    SolidScatterItem* m_solidItem = nullptr;
    QString m_solidTexturePath;
    qreal m_solidFillRatio = 0.0;
    QString m_solidChemicalId;
};
#pragma once

#include "LabItem.h"
#include <QPainter>

class WideBottleItem;
class QGraphicsPixmapItem;

class TweezersItem : public LabItem
{
    Q_OBJECT

public:
    explicit TweezersItem(QGraphicsItem* parent = nullptr);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

private:
    void tryPickSolid();
    void tryDropSolid();
    void refreshCarriedPreview();
    QPointF tipLocalPos() const;
    QPointF tipScenePos() const;

private:
    QGraphicsPixmapItem* m_carriedPreview = nullptr;
    QString m_carriedChemicalId;
    QString m_carriedTexturePath;
};
#pragma once
#include "AbstractLiquidContainerItem.h"

class NarrowBottleItem;
class BeakerPourHandleItem;

class BeakerItem : public AbstractLiquidContainerItem
{
    Q_OBJECT

public:
    explicit BeakerItem(QGraphicsItem* parent = nullptr);
    ~BeakerItem() override;

    void setPourHandleIconPath(const QString& iconPath);

protected:
    QRectF liquidRectLocal() const override;
    QColor defaultLiquidColor() const override;
    qreal defaultLiquidFillRatio() const override;
    QPainterPath liquidClipPathLocal() const override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;
    void timerEvent(QTimerEvent* event) override;

private:
    void updateAttachmentScan();
    void attachBottle(NarrowBottleItem* bottle);
    void detachBottle();
    void refreshAttachedBottleTransform();
    void refreshHandleGeometry();

    QPointF beakerAttachPivotLocalPos() const;
    qreal maxPourRotationDeg() const;
    qreal previewPourRatio() const;

private:
    NarrowBottleItem* m_attachedBottle = nullptr;
    BeakerPourHandleItem* m_pourHandle = nullptr;

    bool m_trackVisible = false;
    qreal m_pourRatio = 0.0;

    int m_scanTimerId = -1;

    NarrowBottleItem* m_detachSuppressedBottle = nullptr;
    qreal m_reacquireDistance = 90.0;
};
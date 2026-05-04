#pragma once
#include "AbstractLiquidContainerItem.h"
#include "ExperimentSidebarConfigLoader.h"
#include <QColor>

class NarrowBottleItem;
class BeakerPourHandleItem;

class BeakerItem : public AbstractLiquidContainerItem
{
    Q_OBJECT

public:
    explicit BeakerItem(QGraphicsItem* parent = nullptr);
    ~BeakerItem() override;

    void setPourHandleIconPath(const QString& iconPath);
    static BeakerItem* createInstance(bool enableLiquid = true,
        const QColor& liquidColor = QColor(),
        QGraphicsItem* parent = nullptr);


    static void setReactionTemplates(const QList<ReactionTemplate>& templates);
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

    QRectF attachZoneSceneRect() const;
    QPointF beakerAttachPivotLocalPos() const;
    qreal maxPourRotationDeg() const;
    qreal previewPourRatio() const;

    void tryApplyReactions();
    void startLiquidColorTransition(const QColor& targetColor);
private:
    NarrowBottleItem* m_attachedBottle = nullptr;
    BeakerPourHandleItem* m_pourHandle = nullptr;

    bool m_trackVisible = false;
    qreal m_pourRatio = 0.0;
    qreal m_currentLiquidLevel = 0.0;


    int m_scanTimerId = -1;

    QColor m_animStartLiquidColor;
    QColor m_animTargetLiquidColor;
    int m_colorAnimTimerId = -1;
    int m_colorAnimStep = 0;

    static QList<ReactionTemplate> s_reactionTemplates;
};
#pragma once

#include <QGraphicsObject>
#include <QColor>
#include <QPainterPath>

class QTimer;
class QVariantAnimation;

class LiquidItem : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit LiquidItem(QGraphicsItem* parent = nullptr);
    ~LiquidItem() override;

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    void setContainerRect(const QRectF& rect);
    void setClipPath(const QPainterPath& path);

    void setFillRatio(qreal ratio);
    qreal fillRatio() const;

    void setColor(const QColor& color);
    QColor color() const;

    void startColorTransition(const QColor& targetColor, int durationMs = 2000);

    void setWaveEnabled(bool enabled);

private slots:
    void tick();

private:
    QPainterPath buildLiquidPath() const;

private:
    QRectF m_containerRect;
    QPainterPath m_clipPath;

    QColor m_color = QColor(80, 140, 255, 150);
    qreal m_fillRatio = 0.55;

    bool m_waveEnabled = true;

    qreal m_wavePhase = 0.0;
    qreal m_waveVelocity = 0.0;
    qreal m_waveAmplitude = 0.0;
    qreal m_surfaceTilt = 0.0;
    qreal m_lastParentRotation = 0.0;
    QPointF m_lastParentScenePos;
    bool m_hasLastParentScenePos = false;

    QTimer* m_timer = nullptr;
    QVariantAnimation* m_colorAnimation = nullptr;
};
#include "LiquidItem.h"

#include <QPainter>
#include <QTimer>
#include <QVariantAnimation>
#include <QtMath>

LiquidItem::LiquidItem(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(QGraphicsItem::ItemStacksBehindParent, false);
    setZValue(0.5);

    m_timer = new QTimer(this);
    m_timer->setInterval(16);
    connect(m_timer, &QTimer::timeout, this, &LiquidItem::tick);
    m_timer->start();

    m_colorAnimation = new QVariantAnimation(this);
    connect(m_colorAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        m_color = value.value<QColor>();
        update();
        });
}

LiquidItem::~LiquidItem()
{
}

QRectF LiquidItem::boundingRect() const
{
    return m_containerRect;
}

void LiquidItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem*,
    QWidget*)
{
    if (m_containerRect.isEmpty() || m_clipPath.isEmpty()) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setClipPath(m_clipPath);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_color);
    painter->drawPath(buildLiquidPath());
    painter->restore();
}

void LiquidItem::setContainerRect(const QRectF& rect)
{
    prepareGeometryChange();
    m_containerRect = rect;
    update();
}

void LiquidItem::setClipPath(const QPainterPath& path)
{
    m_clipPath = path;
    update();
}

void LiquidItem::setFillRatio(qreal ratio)
{
    m_fillRatio = qBound(0.0, ratio, 1.0);
    update();
}

qreal LiquidItem::fillRatio() const
{
    return m_fillRatio;
}

void LiquidItem::setColor(const QColor& color)
{
    m_color = color;
    update();
}

QColor LiquidItem::color() const
{
    return m_color;
}

void LiquidItem::startColorTransition(const QColor& targetColor, int durationMs)
{
    if (!m_colorAnimation) {
        return;
    }

    m_colorAnimation->stop();
    m_colorAnimation->setDuration(durationMs);
    m_colorAnimation->setStartValue(m_color);
    m_colorAnimation->setEndValue(targetColor);
    m_colorAnimation->start();
}

void LiquidItem::setWaveEnabled(bool enabled)
{
    m_waveEnabled = enabled;
}

void LiquidItem::tick()
{
    const QGraphicsItem* p = parentItem();
    if (!p) {
        update();
        return;
    }

    const qreal parentRotation = p->rotation();
    const qreal rotationDelta = parentRotation - m_lastParentRotation;
    m_lastParentRotation = parentRotation;

    const QPointF parentScenePos = p->mapToScene(QPointF(0.0, 0.0));
    QPointF sceneDelta(0.0, 0.0);

    if (m_hasLastParentScenePos) {
        sceneDelta = parentScenePos - m_lastParentScenePos;
    }
    else {
        m_hasLastParentScenePos = true;
    }
    m_lastParentScenePos = parentScenePos;

    if (!m_waveEnabled) {
        update();
        return;
    }

    // 旋转驱动：保留你原来的“水平面趋向世界水平”的效果
    const qreal rotationTiltTarget = -parentRotation * 0.18;

    // 平移驱动：左右移动时给液面一个瞬时倾斜目标
    const qreal translationTiltTarget = qBound(-10.0, -sceneDelta.x() * 1.2, 10.0);

    const qreal targetTilt = rotationTiltTarget + translationTiltTarget;
    m_surfaceTilt += (targetTilt - m_surfaceTilt) * 0.16;

    // 两种激励共同驱动液面波相位
    m_waveVelocity += rotationDelta * 0.05;
    m_waveVelocity += sceneDelta.x() * 0.003;
    m_waveVelocity *= 0.94;
    m_wavePhase += m_waveVelocity;

    // 两种激励共同驱动波幅
    m_waveAmplitude += qAbs(rotationDelta) * 0.20;
    m_waveAmplitude += qAbs(sceneDelta.x()) * 0.11;
    m_waveAmplitude *= 0.93;

    const qreal maxAmplitude = m_containerRect.height() * 0.08;
    m_waveAmplitude = qBound(0.0, m_waveAmplitude, maxAmplitude);

    update();
}

QPainterPath LiquidItem::buildLiquidPath() const
{
    QPainterPath path;
    if (m_containerRect.isEmpty()) {
        return path;
    }

    const qreal left = m_containerRect.left();
    const qreal right = m_containerRect.right();
    const qreal bottom = m_containerRect.bottom();
    const qreal width = m_containerRect.width();
    const qreal height = m_containerRect.height();

    const qreal baseY = bottom - height * m_fillRatio;

    path.moveTo(left, bottom);

    const int steps = 24;
    for (int i = 0; i <= steps; ++i) {
        const qreal t = static_cast<qreal>(i) / static_cast<qreal>(steps);
        const qreal x = left + width * t;

        const qreal centered = (t - 0.5) * 2.0;
        const qreal tiltOffset = centered * m_surfaceTilt;
        const qreal waveOffset = qSin(m_wavePhase + t * M_PI * 2.0) * m_waveAmplitude;

        const qreal y = baseY + tiltOffset + waveOffset;

        if (i == 0) {
            path.lineTo(x, y);
        }
        else {
            path.lineTo(x, y);
        }
    }

    path.lineTo(right, bottom);
    path.closeSubpath();

    return path;
}
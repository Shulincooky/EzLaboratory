#include "SolidScatterItem.h"

#include <QPainter>
#include <QRandomGenerator>
#include <QtMath>
#include <algorithm>

SolidScatterItem::SolidScatterItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);
    setZValue(0.25); // 在 LiquidItem(0.5) 下方
    m_randomSeed = QRandomGenerator::global()->generate();
}

QRectF SolidScatterItem::boundingRect() const
{
    return m_containerRect;
}

void SolidScatterItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem*,
    QWidget*)
{
    if (m_texture.isNull() || m_instances.isEmpty() || m_containerRect.isEmpty()) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (!m_clipPath.isEmpty()) {
        painter->setClipPath(m_clipPath);
    }

    const QRectF sourceRect(QPointF(0.0, 0.0), QSizeF(m_texture.width(), m_texture.height()));

    for (const Instance& instance : m_instances) {
        painter->save();

        const QPointF c = instance.targetRect.center();
        painter->translate(c);
        painter->rotate(instance.rotationDeg);
        painter->translate(-c);

        painter->drawPixmap(instance.targetRect, m_texture, sourceRect);
        painter->restore();
    }

    painter->restore();
}

void SolidScatterItem::setContainerRect(const QRectF& rect)
{
    if (m_containerRect == rect) {
        return;
    }

    prepareGeometryChange();
    m_containerRect = rect;
    rebuildInstances();
    update();
}

void SolidScatterItem::setClipPath(const QPainterPath& path)
{
    m_clipPath = path;
    rebuildInstances();
    update();
}

void SolidScatterItem::setTexturePath(const QString& texturePath)
{
    if (m_texturePath == texturePath) {
        return;
    }

    m_texturePath = texturePath;
    m_texture = QPixmap(texturePath);
    rebuildInstances();
    update();
}

QString SolidScatterItem::texturePath() const
{
    return m_texturePath;
}

void SolidScatterItem::setFillRatio(qreal ratio)
{
    ratio = qBound(0.0, ratio, 1.0);
    if (qFuzzyCompare(m_fillRatio, ratio)) {
        return;
    }

    m_fillRatio = ratio;
    rebuildInstances();
    update();
}

qreal SolidScatterItem::fillRatio() const
{
    return m_fillRatio;
}

void SolidScatterItem::setRandomSeed(quint32 seed)
{
    if (m_randomSeed == seed) {
        return;
    }

    m_randomSeed = seed;
    rebuildInstances();
    update();
}

quint32 SolidScatterItem::randomSeed() const
{
    return m_randomSeed;
}

void SolidScatterItem::rebuildInstances()
{
    m_instances.clear();

    if (m_texture.isNull() || m_fillRatio <= 0.0 || m_containerRect.isEmpty()) {
        return;
    }

    QRectF usableRect = m_containerRect;
    if (!m_clipPath.isEmpty()) {
        usableRect = usableRect.intersected(m_clipPath.boundingRect());
    }

    if (usableRect.isEmpty()) {
        return;
    }

    const qreal visibleHeight = usableRect.height() * m_fillRatio;
    if (visibleHeight <= 2.0) {
        return;
    }

    QRectF pileRect(
        usableRect.left(),
        usableRect.bottom() - visibleHeight,
        usableRect.width(),
        visibleHeight);

    if (pileRect.isEmpty()) {
        return;
    }

    QRandomGenerator rng(m_randomSeed);

    // 填充越多，实例越多；全部绘制在同一个 item 中，避免一堆独立子图元
    const int count = qBound(8, static_cast<int>(qRound(14.0 + m_fillRatio * 42.0)), 72);

    const qreal textureAspect =
        (m_texture.width() > 0)
        ? (static_cast<qreal>(m_texture.height()) / static_cast<qreal>(m_texture.width()))
        : 1.0;

    const qreal baseWidth = qMax(10.0, pileRect.width() * 0.18);
    const qreal maxWidth = qMax(baseWidth, pileRect.width() * 0.32);

    for (int i = 0; i < count; ++i) {
        const qreal scale = rng.generateDouble() * 0.55 + 0.45; // 0.45 ~ 1.0
        const qreal w = qBound(8.0, baseWidth * scale, maxWidth);
        const qreal h = qMax(8.0, w * textureAspect);

        // 允许轻微越界，最终靠 clipPath 裁切掉边缘
        const qreal xMin = pileRect.left() - w * 0.20;
        const qreal xMax = pileRect.right() - w * 0.80;
        const qreal x = xMin + (xMax - xMin) * rng.generateDouble();

        // 用平方分布把更多实例压到下部，看起来更像堆积
        const qreal t = rng.generateDouble();
        const qreal bottomBias = t * t;

        const qreal availableY = qMax(0.0, pileRect.height() - h);
        const qreal y = pileRect.bottom() - h - availableY * bottomBias;

        Instance instance;
        instance.targetRect = QRectF(x, y, w, h);
        instance.rotationDeg = -24.0 + 48.0 * rng.generateDouble();

        m_instances.push_back(instance);
    }

    std::sort(m_instances.begin(), m_instances.end(), [](const Instance& a, const Instance& b) {
        return a.targetRect.center().y() < b.targetRect.center().y();
        });
}
#include "AbstractLiquidContainerItem.h"
#include "LiquidItem.h"

#include <QImage>
#include <QPainterPath>
#include <QVariantAnimation>
#include <QRegion>

namespace
{
    QPainterPath buildPathFromAlphaImage(const QImage& image)
    {
        QPainterPath path;

        for (int y = 0; y < image.height(); ++y) {
            int runStart = -1;

            for (int x = 0; x < image.width(); ++x) {
                const int alpha = qAlpha(image.pixel(x, y));
                const bool solid = alpha > 8;

                if (solid && runStart < 0) {
                    runStart = x;
                }
                else if (!solid && runStart >= 0) {
                    path.addRect(QRectF(runStart, y, x - runStart, 1));
                    runStart = -1;
                }
            }

            if (runStart >= 0) {
                path.addRect(QRectF(runStart, y, image.width() - runStart, 1));
            }
        }

        return path.simplified();
    }
}

AbstractLiquidContainerItem::AbstractLiquidContainerItem(const QString& itemType,
    const QString& itemName,
    const QString& imagePath,
    const QSizeF& itemSize,
    QGraphicsItem* parent)
    : LabItem(itemType, itemName, imagePath, itemSize, parent)
{
}

AbstractLiquidContainerItem::~AbstractLiquidContainerItem()
{
}

void AbstractLiquidContainerItem::setLiquidColor(const QColor& color)
{
    if (!m_liquidItem) {
        return;
    }

    m_liquidItem->setColor(color);
}

void AbstractLiquidContainerItem::startLiquidColorTransition(const QColor& targetColor, int durationMs)
{
    if (!m_liquidItem) {
        return;
    }

    m_liquidItem->startColorTransition(targetColor, durationMs);
}

void AbstractLiquidContainerItem::setLiquidFillRatio(qreal ratio)
{
    if (!m_liquidItem) {
        return;
    }

    m_liquidItem->setFillRatio(ratio);
}

void AbstractLiquidContainerItem::startContainerSway(qreal maxAngleDeg, int durationMs)
{
    if (!m_liquidRenderingEnabled)
        return;
    if (!m_swayAnimation) {
        m_swayAnimation = new QVariantAnimation(this);
        connect(m_swayAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
            setRotation(value.toReal());
            });
    }

    m_swayAnimation->stop();
    m_swayAnimation->setDuration(durationMs);
    m_swayAnimation->setKeyValueAt(0.0, 0.0);
    m_swayAnimation->setKeyValueAt(0.20, -maxAngleDeg);
    m_swayAnimation->setKeyValueAt(0.45, maxAngleDeg * 0.65);
    m_swayAnimation->setKeyValueAt(0.70, -maxAngleDeg * 0.35);
    m_swayAnimation->setKeyValueAt(1.0, 0.0);
    m_swayAnimation->start();
}

void AbstractLiquidContainerItem::ensureLiquidCreated()
{
    if (m_liquidItem) {
        refreshLiquidGeometry();
        return;
    }

    LiquidItem* liquid = createLiquidItem();
    if (!liquid) {
        return;
    }

    initializeLiquid(liquid);
}

void AbstractLiquidContainerItem::destroyLiquid()
{
    if (!m_liquidItem) {
        return;
    }

    delete m_liquidItem;
    m_liquidItem = nullptr;
}

LiquidItem* AbstractLiquidContainerItem::createLiquidItem()
{
    return new LiquidItem(this);
}

void AbstractLiquidContainerItem::initializeLiquid(LiquidItem* liquid)
{
    if (!liquid) {
        return;
    }

    m_liquidItem = liquid;
    m_liquidRenderingEnabled = true;
    m_liquidItem->setParentItem(this);
    m_liquidItem->setColor(defaultLiquidColor());
    m_liquidItem->setFillRatio(defaultLiquidFillRatio());

    refreshLiquidGeometry();
}

void AbstractLiquidContainerItem::refreshLiquidGeometry()
{
    if (!m_liquidItem) {
        return;
    }

    m_liquidItem->setContainerRect(liquidRectLocal());
    m_liquidItem->setClipPath(liquidClipPathLocal());
}

QColor AbstractLiquidContainerItem::defaultLiquidColor() const
{
    return QColor(80, 140, 255, 140);
}

qreal AbstractLiquidContainerItem::defaultLiquidFillRatio() const
{
    return 0.55;
}

void AbstractLiquidContainerItem::setLiquidLevel(qreal level)
{
    setLiquidFillRatio(level);
}

void AbstractLiquidContainerItem::setLiquidRenderingEnabled(bool enabled)
{
    if (m_liquidRenderingEnabled == enabled) {
        return;
    }

    m_liquidRenderingEnabled = enabled;

    if (m_liquidRenderingEnabled) {
        ensureLiquidCreated();
    }
    else {
        destroyLiquid();
    }
}

bool AbstractLiquidContainerItem::liquidRenderingEnabled() const
{
    return m_liquidRenderingEnabled;
}

bool AbstractLiquidContainerItem::hasLiquidItem() const
{
    return m_liquidItem != nullptr;
}

QPainterPath AbstractLiquidContainerItem::buildAlphaClipPath() const
{
    const QPixmap& pix = itemPixmap();
    if (pix.isNull()) {
        QPainterPath fallback;
        fallback.addRect(liquidRectLocal());
        return fallback;
    }

    const QSize targetSize(qMax(1, qRound(itemSize().width())), qMax(1, qRound(itemSize().height())));
    QImage scaled = pix.toImage().scaled(
        targetSize,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32);

    return buildPathFromAlphaImage(scaled);
}

QPainterPath AbstractLiquidContainerItem::liquidClipPathLocal() const
{
    QPainterPath path;
    path.addRect(liquidRectLocal());
    return path;
}

#include "WideBottleItem.h"
#include "WideBottlePlugItem.h"
#include "BottleLabelItem.h"

#include <QPainter>
#include <QPixmap>
#include <QRandomGenerator>
#include <algorithm>

namespace
{
    constexpr const char* kBottleBodyImagePath =
        ":/EzLaboratory/resources/image/glassware/generic/wide-mouth_bottle/bottle.png";

    class SolidScatterItem : public QGraphicsItem
    {
    public:
        explicit SolidScatterItem(QGraphicsItem* parent = nullptr)
            : QGraphicsItem(parent)
        {
            setAcceptedMouseButtons(Qt::NoButton);
            setAcceptHoverEvents(false);
            setZValue(0.25);
            m_randomSeed = QRandomGenerator::global()->generate();
        }

        QRectF boundingRect() const override
        {
            return m_containerRect;
        }

        void paint(QPainter* painter,
            const QStyleOptionGraphicsItem*,
            QWidget*) override
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

        void setContainerRect(const QRectF& rect)
        {
            if (m_containerRect == rect) {
                return;
            }

            prepareGeometryChange();
            m_containerRect = rect;
            rebuildInstances();
            update();
        }

        void setClipPath(const QPainterPath& path)
        {
            m_clipPath = path;
            rebuildInstances();
            update();
        }

        void setTexturePath(const QString& texturePath)
        {
            if (m_texturePath == texturePath) {
                return;
            }

            m_texturePath = texturePath;
            m_texture = QPixmap(texturePath);
            rebuildInstances();
            update();
        }

        void setFillRatio(qreal ratio)
        {
            ratio = qBound(0.0, ratio, 1.0);
            if (qFuzzyCompare(m_fillRatio, ratio)) {
                return;
            }

            m_fillRatio = ratio;
            rebuildInstances();
            update();
        }

    private:
        struct Instance
        {
            QRectF targetRect;
            qreal rotationDeg = 0.0;
        };

        void rebuildInstances()
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

            const QRectF pileRect(
                usableRect.left(),
                usableRect.bottom() - visibleHeight,
                usableRect.width(),
                visibleHeight);

            if (pileRect.isEmpty()) {
                return;
            }

            QRandomGenerator rng(m_randomSeed);

            const int count = qBound(8, static_cast<int>(qRound(14.0 + m_fillRatio * 42.0)), 72);

            const qreal textureAspect =
                (m_texture.width() > 0)
                ? (static_cast<qreal>(m_texture.height()) / static_cast<qreal>(m_texture.width()))
                : 1.0;

            const qreal baseWidth = qMax(10.0, pileRect.width() * 0.18);
            const qreal maxWidth = qMax(baseWidth, pileRect.width() * 0.32);

            for (int i = 0; i < count; ++i) {
                const qreal scale = rng.generateDouble() * 0.55 + 0.45;
                const qreal w = qBound(8.0, baseWidth * scale, maxWidth);
                const qreal h = qMax(8.0, w * textureAspect);

                const qreal xMin = pileRect.left() - w * 0.20;
                const qreal xMax = pileRect.right() - w * 0.80;
                const qreal x = xMin + (xMax - xMin) * rng.generateDouble();

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

    private:
        QRectF m_containerRect;
        QPainterPath m_clipPath;

        QString m_texturePath;
        QPixmap m_texture;

        qreal m_fillRatio = 0.0;
        quint32 m_randomSeed = 0;

        QVector<Instance> m_instances;
    };
}

WideBottleItem::WideBottleItem(QGraphicsItem* parent)
    : AbstractBottleItem(
        "wide_bottle",
        "WideBottle",
        kBottleBodyImagePath,
        QSizeF(108, 180),
        parent)
{
    initializePlug(new WideBottlePlugItem(this));
    initializeLabel(new BottleLabelItem(this));
    setLiquidRenderingEnabled(false);
}

WideBottleItem::~WideBottleItem()
{
    destroySolid();
}

WideBottleItem* WideBottleItem::createInstance(const BottleLabelData& data,
    bool enableLiquid,
    const QColor& liquidColor,
    bool enableSolid,
    const QString& solidTexturePath,
    qreal solidFillRatio,
    QGraphicsItem* parent)
{
    auto* bottle = new WideBottleItem(parent);
    bottle->finalizeInstance(data);

    bottle->setLiquidRenderingEnabled(enableLiquid);
    if (enableLiquid && liquidColor.isValid()) {
        bottle->setLiquidColor(liquidColor);
    }

    if (enableSolid) {
        bottle->setSolidTexturePath(solidTexturePath);
        bottle->setSolidFillRatio(solidFillRatio);
        bottle->setSolidRenderingEnabled(true);
    }

    return bottle;
}

QPointF WideBottleItem::mouthLocalPos() const
{
    return QPointF(54.0, 15.0);
}

qreal WideBottleItem::snapRadius() const
{
    return 42.0;
}

qreal WideBottleItem::plugInsertOffsetY() const
{
    return 24.0;
}

QPointF WideBottleItem::labelLocalPos() const
{
    return QPointF(20.0, 76.0);
}

QSizeF WideBottleItem::labelLogicalSize() const
{
    return QSizeF(60.0, 38.0);
}

QRectF WideBottleItem::liquidRectLocal() const
{
    const qreal leftInset = 10.0;
    const qreal rightInset = 10.0;
    const qreal topInset = 80.0;
    const qreal bottomInset = 14.0;

    return QRectF(
        leftInset,
        topInset,
        itemSize().width() - leftInset - rightInset,
        itemSize().height() - topInset - bottomInset
    );
}

QColor WideBottleItem::defaultLiquidColor() const
{
    return QColor(90, 150, 255, 125);
}

qreal WideBottleItem::defaultLiquidFillRatio() const
{
    return 1.0;
}

void WideBottleItem::setSolidTexturePath(const QString& texturePath)
{
    if (m_solidTexturePath == texturePath) {
        return;
    }

    m_solidTexturePath = texturePath;

    if (m_solidTexturePath.isEmpty()) {
        destroySolid();
        return;
    }

    ensureSolidCreated();
    if (m_solidItem) {
        m_solidItem->setTexturePath(m_solidTexturePath);
        refreshSolidGeometry();
    }
}

QString WideBottleItem::solidTexturePath() const
{
    return m_solidTexturePath;
}

void WideBottleItem::setSolidFillRatio(qreal ratio)
{
    ratio = qBound(0.0, ratio, 1.0);
    if (qFuzzyCompare(m_solidFillRatio, ratio)) {
        return;
    }

    m_solidFillRatio = ratio;

    if (m_solidFillRatio <= 0.0) {
        destroySolid();
        return;
    }

    ensureSolidCreated();
    if (m_solidItem) {
        m_solidItem->setFillRatio(m_solidFillRatio);
        refreshSolidGeometry();
    }
}

qreal WideBottleItem::solidFillRatio() const
{
    return m_solidFillRatio;
}

void WideBottleItem::setSolidRenderingEnabled(bool enabled)
{
    if (!enabled) {
        destroySolid();
        return;
    }

    if (m_solidTexturePath.isEmpty()) {
        return;
    }

    if (m_solidFillRatio <= 0.0) {
        m_solidFillRatio = 1.0;
    }

    ensureSolidCreated();
    if (m_solidItem) {
        m_solidItem->setTexturePath(m_solidTexturePath);
        m_solidItem->setFillRatio(m_solidFillRatio);
        refreshSolidGeometry();
    }
}

bool WideBottleItem::solidRenderingEnabled() const
{
    return m_solidItem != nullptr;
}

bool WideBottleItem::hasSolidItem() const
{
    return m_solidItem != nullptr;
}

void WideBottleItem::ensureSolidCreated()
{
    if (m_solidItem) {
        refreshSolidGeometry();
        return;
    }

    if (m_solidTexturePath.isEmpty() || m_solidFillRatio <= 0.0) {
        return;
    }

    m_solidItem = new SolidScatterItem(this);
    m_solidItem->setParentItem(this);
    m_solidItem->setTexturePath(m_solidTexturePath);
    m_solidItem->setFillRatio(m_solidFillRatio);
    m_solidItem->setZValue(0.25);

    refreshSolidGeometry();
}

void WideBottleItem::destroySolid()
{
    if (!m_solidItem) {
        return;
    }

    delete m_solidItem;
    m_solidItem = nullptr;
}

void WideBottleItem::refreshSolidGeometry()
{
    if (!m_solidItem) {
        return;
    }

    m_solidItem->setContainerRect(liquidRectLocal());
    m_solidItem->setClipPath(liquidClipPathLocal());
}
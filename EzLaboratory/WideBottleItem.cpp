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

void WideBottleItem::setSolidChemicalId(const QString& chemicalId)
{
    m_solidChemicalId = chemicalId;
    clearContainedChemicalIds();
    if (!chemicalId.isEmpty()) {
        addContainedChemicalId(chemicalId);
    }
}

QString WideBottleItem::solidChemicalId() const
{
    return m_solidChemicalId;
}

bool WideBottleItem::takeSolidForTweezers(QString* chemicalId, QString* texturePath)
{
    if (hasPlug()) {
        return false;
    }

    if (!m_solidItem || m_solidChemicalId.isEmpty() || m_solidTexturePath.isEmpty()) {
        return false;
    }

    if (chemicalId) {
        *chemicalId = m_solidChemicalId;
    }
    if (texturePath) {
        *texturePath = m_solidTexturePath;
    }

    return true;
}
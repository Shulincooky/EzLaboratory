#include "AbstractBottleItem.h"
#include "AbstractPlugItem.h"
#include "BottleLabelItem.h"

AbstractBottleItem::AbstractBottleItem(const QString& itemType,
    const QString& itemName,
    const QString& imagePath,
    const QSizeF& itemSize,
    QGraphicsItem* parent)
    : AbstractLiquidContainerItem(itemType, itemName, imagePath, itemSize, parent)
{
}

AbstractBottleItem::~AbstractBottleItem()
{
}

QPointF AbstractBottleItem::mouthScenePos() const
{
    return mapToScene(mouthLocalPos());
}

QPointF AbstractBottleItem::plugLocalTopLeft(const AbstractPlugItem* plug) const
{
    if (!plug) {
        return QPointF();
    }

    const QRectF plugRect = plug->boundingRect();
    const QPointF mouth = mouthLocalPos();

    return QPointF(
        mouth.x() - plugRect.width() * 0.5 + plugOffsetX(),
        mouth.y() - plugRect.height() + plugInsertOffsetY()
    );
}

void AbstractBottleItem::setPlugReference(AbstractPlugItem* plug)
{
    m_plug = plug;
}

void AbstractBottleItem::clearPlugReference(AbstractPlugItem* plug)
{
    if (m_plug == plug) {
        m_plug = nullptr;
    }
}

bool AbstractBottleItem::hasPlug() const
{
    return m_plug != nullptr;
}

AbstractPlugItem* AbstractBottleItem::plug() const
{
    return m_plug;
}

void AbstractBottleItem::initializePlug(AbstractPlugItem* plug)
{
    if (!plug) {
        return;
    }

    plug->attachToBottle(this);
}

void AbstractBottleItem::initializeLabel(BottleLabelItem* label)
{
    if (!label) {
        return;
    }

    m_labelItem = label;
    m_labelItem->setParentItem(this);
    m_labelItem->setPos(labelLocalPos() + m_labelOffset);
}

void AbstractBottleItem::setLabelData(const BottleLabelData& data)
{
    m_labelData = data;

    if (!m_labelItem) {
        return;
    }

    m_labelItem->updateLabel(data, labelLogicalSize(), labelTemplatePath());
    m_labelItem->setPos(labelLocalPos() + m_labelOffset);
}
void AbstractBottleItem::setLabelOffset(const QPointF& offset)
{
    m_labelOffset = offset;

    if (m_labelItem) {
        m_labelItem->setPos(labelLocalPos() + m_labelOffset);
    }
}

QPointF AbstractBottleItem::labelOffset() const
{
    return m_labelOffset;
}

QPointF AbstractBottleItem::pourPivotLocalPos() const
{
    return mouthLocalPos();
}

BottleLabelData AbstractBottleItem::labelData() const
{
    return m_labelData;
}

qreal AbstractBottleItem::plugOffsetX() const
{
    return 0.0;
}

QString AbstractBottleItem::labelTemplatePath() const
{
    return QStringLiteral(":/EzLaboratory/resources/label.svg");
}

void AbstractBottleItem::finalizeInstance(const BottleLabelData& data)
{
    setLabelData(data);
}

QColor AbstractBottleItem::defaultLiquidColor() const
{
    return QColor(90, 150, 255, 125);
}

qreal AbstractBottleItem::defaultLiquidFillRatio() const
{
    // 瓶子默认“装满到瓶颈以下的液体区域”
    return 1.0;
}

QPainterPath AbstractBottleItem::liquidClipPathLocal() const
{
    QPainterPath path;
    path.addRect(liquidRectLocal());
    return path;
}
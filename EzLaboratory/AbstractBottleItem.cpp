#include "AbstractBottleItem.h"
#include "AbstractPlugItem.h"

AbstractBottleItem::AbstractBottleItem(const QString& itemType,
    const QString& itemName,
    const QString& imagePath,
    const QSizeF& itemSize,
    QGraphicsItem* parent)
    : LabItem(itemType, itemName, imagePath, itemSize, parent)
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

qreal AbstractBottleItem::plugOffsetX() const
{
    return 0.0;
}
#include "LabItem.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>

LabItem::LabItem(const QString& itemName,
    const QString& imagePath,
    const QSizeF& itemSize,
    QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_itemName(itemName)
    , m_imagePath(imagePath)
    , m_itemSize(itemSize)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setAcceptedMouseButtons(Qt::LeftButton);

    if (!m_imagePath.isEmpty()) {
        m_pixmap.load(m_imagePath);
        if (m_pixmap.isNull()) {
            qWarning() << "Failed to load pixmap for item:" << m_itemName
                << "path:" << m_imagePath;
        }
    }
}

QRectF LabItem::boundingRect() const
{
    return QRectF(0, 0, m_itemSize.width(), m_itemSize.height());
}

void LabItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem*,
    QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (!m_pixmap.isNull()) {
        painter->drawPixmap(boundingRect().toRect(), m_pixmap);
    }
    else {
        painter->setPen(QPen(QColor(50, 90, 180), 2));
        painter->setBrush(QBrush(QColor(180, 220, 255)));
        painter->drawRoundedRect(boundingRect(), 8, 8);

        painter->setPen(Qt::black);
        painter->drawText(boundingRect(), Qt::AlignCenter, m_itemName);
    }

    if (isSelected()) {
        painter->setPen(QPen(QColor(255, 170, 0), 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
}

QString LabItem::itemName() const
{
    return m_itemName;
}

QString LabItem::imagePath() const
{
    return m_imagePath;
}

void LabItem::setItemPixmap(const QString& imagePath)
{
    m_imagePath = imagePath;
    m_pixmap.load(m_imagePath);
    update();
}

void LabItem::setItemSize(const QSizeF& size)
{
    prepareGeometryChange();
    m_itemSize = size;
    update();
}

QSizeF LabItem::itemSize() const
{
    return m_itemSize;
}

QVariant LabItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        QRectF sceneRect = scene()->sceneRect();
        QRectF itemRect = boundingRect();

        const qreal minX = sceneRect.left();
        const qreal minY = sceneRect.top();
        const qreal maxX = sceneRect.right() - itemRect.width();
        const qreal maxY = sceneRect.bottom() - itemRect.height();

        newPos.setX(qBound(minX, newPos.x(), maxX));
        newPos.setY(qBound(minY, newPos.y(), maxY));

        return newPos;
    }

    return QGraphicsObject::itemChange(change, value);
}
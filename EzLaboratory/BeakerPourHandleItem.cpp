#include "BeakerPourHandleItem.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

BeakerPourHandleItem::BeakerPourHandleItem(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setZValue(50.0);
}

QRectF BeakerPourHandleItem::boundingRect() const
{
    return QRectF(-m_size * 0.5, -m_size * 0.5, m_size, m_size);
}

void BeakerPourHandleItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem*,
    QWidget*)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (!m_icon.isNull()) {
        painter->drawPixmap(boundingRect().toRect(), m_icon);
    }
    else {
        painter->setPen(QPen(QColor(70, 70, 70), 1.5));
        painter->setBrush(QBrush(QColor(250, 250, 250)));
        painter->drawEllipse(boundingRect());

        QPainterPath mark;
        mark.moveTo(-3.0, -4.0);
        mark.lineTo(4.0, 0.0);
        mark.lineTo(-3.0, 4.0);
        painter->setBrush(QBrush(QColor(70, 70, 70)));
        painter->drawPath(mark);
    }

    painter->restore();
}

void BeakerPourHandleItem::setTrackGeometry(qreal x, qreal topY, qreal bottomY)
{
    m_trackX = x;
    m_trackTopY = qMin(topY, bottomY);
    m_trackBottomY = qMax(topY, bottomY);
    updatePosFromRatio();
}

void BeakerPourHandleItem::setRatio(qreal ratio)
{
    m_ratio = qBound(0.0, ratio, 1.0);
    updatePosFromRatio();
}

qreal BeakerPourHandleItem::ratio() const
{
    return m_ratio;
}

void BeakerPourHandleItem::setIconPath(const QString& iconPath)
{
    m_icon = QPixmap(iconPath);
    update();
}

void BeakerPourHandleItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit dragStarted();
    event->accept();
}

void BeakerPourHandleItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    const QPointF p = mapToParent(event->pos());
    const qreal clampedY = qBound(m_trackTopY, p.y(), m_trackBottomY);

    const qreal height = m_trackBottomY - m_trackTopY;
    qreal t = 0.0;
    if (height > 1e-6) {
        t = (m_trackBottomY - clampedY) / height;
    }

    setRatio(t);
    emit ratioChanged(m_ratio);
    event->accept();
}

void BeakerPourHandleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    emit dragFinished();
    event->accept();
}

void BeakerPourHandleItem::updatePosFromRatio()
{
    const qreal y = m_trackBottomY - (m_trackBottomY - m_trackTopY) * m_ratio;
    setPos(m_trackX, y);
}
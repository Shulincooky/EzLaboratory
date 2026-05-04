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
    const qreal topLocal = m_trackTopY - pos().y();
    const qreal bottomLocal = m_trackBottomY - pos().y();

    const QRectF handleRect(-m_size * 0.5, -m_size * 0.5, m_size, m_size);
    const QRectF trackRect(-2.0, topLocal, 4.0, bottomLocal - topLocal);

    return handleRect.united(trackRect).adjusted(-2.0, -2.0, 2.0, 2.0);
}

void BeakerPourHandleItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem*,
    QWidget*)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QRectF handleRect(-m_size * 0.5, -m_size * 0.5, m_size, m_size);

    if (m_trackVisible) {
        const qreal topLocal = m_trackTopY - pos().y();
        const qreal bottomLocal = m_trackBottomY - pos().y();

        painter->setPen(QPen(QColor(120, 120, 120), 1.5, Qt::DashLine));
        painter->drawLine(QPointF(0.0, topLocal), QPointF(0.0, bottomLocal));
    }

    if (!m_icon.isNull()) {
        painter->drawPixmap(handleRect.toAlignedRect(), m_icon);
    }
    else {
        painter->setPen(QPen(QColor(70, 70, 70), 1.5));
        painter->setBrush(QBrush(QColor(250, 250, 250)));
        painter->drawEllipse(handleRect);

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

void BeakerPourHandleItem::setTrackVisible(bool visible)
{
    if (m_trackVisible == visible) {
        return;
    }

    prepareGeometryChange();
    m_trackVisible = visible;
    update();
}
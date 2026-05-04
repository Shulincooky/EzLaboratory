#include "BottleLabelItem.h"
#include "BottleLabelRenderer.h"

#include <QPainter>

BottleLabelItem::BottleLabelItem(QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent)
{
    setZValue(2.0);
}

void BottleLabelItem::updateLabel(const BottleLabelData& data,
    const QSizeF& logicalSize,
    const QString& templatePath)
{
    const QSize pixelSize(
        qMax(1, qRound(logicalSize.width() * 4.0)),
        qMax(1, qRound(logicalSize.height() * 4.0)));

    QPixmap pix = BottleLabelRenderer::renderLabel(data, pixelSize, templatePath);
    if (pix.isNull()) {
        setPixmap(QPixmap());
        return;
    }

    QPixmap scaledLabel = pix.scaled(
        logicalSize.toSize(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation);

    QPixmap composed(logicalSize.toSize());
    composed.fill(Qt::transparent);

    QPainter painter(&composed);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 先垫一个不透明白底，避免液体从标签透明区域透出来
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(
        QRectF(0.0, 0.0, logicalSize.width(), logicalSize.height()),
        4.0, 4.0);

    // 再画 SVG 标签内容
    painter.drawPixmap(0, 0, scaledLabel);

    setPixmap(composed);
}

void BottleLabelItem::setParentClipRect(const QRectF& rectInParent)
{
    m_parentClipRect = rectInParent;
    update();
}

void BottleLabelItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    painter->save();

    if (!m_parentClipRect.isEmpty()) {
        painter->setClipRect(mapRectFromParent(m_parentClipRect));
    }

    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->restore();
}
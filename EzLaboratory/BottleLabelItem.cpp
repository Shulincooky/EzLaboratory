#include "BottleLabelItem.h"
#include "BottleLabelRenderer.h"

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

    setPixmap(pix.scaled(
        logicalSize.toSize(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation));
}
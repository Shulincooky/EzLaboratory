#include "TweezersItem.h"
#include "WideBottleItem.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QLineF>

namespace
{
    constexpr const char* kTweezersImagePath =
        ":/EzLaboratory/resources/image/lab_tools/tweezers.png";
}

TweezersItem::TweezersItem(QGraphicsItem* parent)
    : LabItem(
        "tweezers",
        "Tweezers",
        kTweezersImagePath,
        QSizeF(150, 150),
        parent)
{
    setZValue(5000.0);

    m_carriedPreview = new QGraphicsPixmapItem(this);
    m_carriedPreview->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    m_carriedPreview->setZValue(-1.0);
    m_carriedPreview->hide();
}

QVariant TweezersItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    const QVariant result = LabItem::itemChange(change, value);

    if (change == QGraphicsItem::ItemPositionHasChanged) {
        if (m_carriedChemicalId.isEmpty()) {
            tryPickSolid();
        }
        refreshCarriedPreview();
    }

    return result;
}

QPointF TweezersItem::tipLocalPos() const
{
    return QPointF(10.0, itemSize().height() - 8.0);
}

QPointF TweezersItem::tipScenePos() const
{
    return mapToScene(tipLocalPos());
}

void TweezersItem::tryPickSolid()
{
    if (!scene() || !m_carriedChemicalId.isEmpty()) {
        return;
    }

    const QPointF tipScene = tipScenePos();

    for (QGraphicsItem* item : scene()->items()) {
        auto* bottle = dynamic_cast<WideBottleItem*>(item);
        if (!bottle) {
            continue;
        }

        if (!bottle->hasSolidItem()) {
            continue;
        }

        const QRectF bottleRect = bottle->sceneBoundingRect();

        const QRectF solidZone(
            bottleRect.left() + 8.0,
            bottleRect.top() + 80.0,
            bottleRect.width() - 16.0,
            bottleRect.height() - 94.0);

        if (!solidZone.contains(tipScene)) {
            const qreal d = QLineF(tipScene, solidZone.center()).length();
            if (d > 26.0) {
                continue;
            }
        }

        QString chemicalId;
        QString texturePath;
        if (bottle->takeSolidForTweezers(&chemicalId, &texturePath)) {
            m_carriedChemicalId = chemicalId;
            m_carriedTexturePath = texturePath;
            refreshCarriedPreview();
            break;
        }
    }
}

void TweezersItem::refreshCarriedPreview()
{
    if (!m_carriedPreview) {
        return;
    }

    if (m_carriedTexturePath.isEmpty()) {
        m_carriedPreview->hide();
        return;
    }

    QPixmap px(m_carriedTexturePath);
    if (px.isNull()) {
        m_carriedPreview->hide();
        return;
    }

    m_carriedPreview->setPixmap(px.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_carriedPreview->setPos(tipLocalPos() - QPointF(8.0, 10.0));
    m_carriedPreview->show();
}

void TweezersItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem*,
    QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    const QPixmap& px = itemPixmap();
    if (!px.isNull()) {
        const QSize targetSize = px.size().scaled(
            boundingRect().size().toSize(),
            Qt::KeepAspectRatio);

        const QRect targetRect(
            static_cast<int>((boundingRect().width() - targetSize.width()) * 0.5),
            static_cast<int>((boundingRect().height() - targetSize.height()) * 0.5),
            targetSize.width(),
            targetSize.height());

        painter->drawPixmap(targetRect, px);
    }
    else {
        painter->setPen(QPen(QColor(50, 90, 180), 2));
        painter->setBrush(QBrush(QColor(180, 220, 255)));
        painter->drawRoundedRect(boundingRect(), 8, 8);

        painter->setPen(Qt::black);
        painter->drawText(boundingRect(), Qt::AlignCenter, itemName());
    }

    if (isSelected()) {
        painter->setPen(QPen(QColor(255, 170, 0), 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
}
#include "TweezersItem.h"
#include "WideBottleItem.h"
#include "BeakerItem.h"

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
        QSizeF(128, 48),
        parent)
{
    m_carriedPreview = new QGraphicsPixmapItem(this);
    m_carriedPreview->setZValue(10.0);
    m_carriedPreview->hide();
}

QVariant TweezersItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    const QVariant result = LabItem::itemChange(change, value);

    if (change == QGraphicsItem::ItemPositionHasChanged) {
        if (m_carriedChemicalId.isEmpty()) {
            tryPickSolid();
        }
        else {
            tryDropSolid();
        }
        refreshCarriedPreview();
    }

    return result;
}

void TweezersItem::tryPickSolid()
{
    if (!scene()) {
        return;
    }

    const QPointF myCenter = mapToScene(boundingRect().center());

    for (QGraphicsItem* item : scene()->items()) {
        auto* bottle = dynamic_cast<WideBottleItem*>(item);
        if (!bottle) {
            continue;
        }

        if (!bottle->hasSolidItem()) {
            continue;
        }

        const qreal d = QLineF(myCenter, bottle->sceneBoundingRect().center()).length();
        if (d > 80.0) {
            continue;
        }

        QString chemicalId;
        QString texturePath;
        if (bottle->takeSolidForTweezers(&chemicalId, &texturePath)) {
            m_carriedChemicalId = chemicalId;
            m_carriedTexturePath = texturePath;
            break;
        }
    }
}

void TweezersItem::tryDropSolid()
{
    if (!scene() || m_carriedChemicalId.isEmpty() || m_carriedTexturePath.isEmpty()) {
        return;
    }

    const QPointF myCenter = mapToScene(boundingRect().center());

    for (QGraphicsItem* item : scene()->items()) {
        auto* beaker = dynamic_cast<BeakerItem*>(item);
        if (!beaker) {
            continue;
        }

        const qreal d = QLineF(myCenter, beaker->sceneBoundingRect().center()).length();
        if (d > 90.0) {
            continue;
        }

        beaker->acceptSolidFromTweezers(m_carriedChemicalId, m_carriedTexturePath);
        m_carriedChemicalId.clear();
        m_carriedTexturePath.clear();
        break;
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

    m_carriedPreview->setPixmap(px.scaled(26, 26, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_carriedPreview->setPos(92.0, 8.0);
    m_carriedPreview->show();
}
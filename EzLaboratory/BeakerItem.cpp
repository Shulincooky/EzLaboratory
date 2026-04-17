#include "BeakerItem.h"
#include "LiquidItem.h"
#include "NarrowBottleItem.h"
#include "BeakerPourHandleItem.h"

#include <QPainterPath>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QTimerEvent>
#include <limits>

namespace
{
    constexpr int kBottleAttachedToBeakerRole = 1001;
    constexpr int kBottleLastMouthScenePosRole = 1002;
    constexpr int kBottleRecentMoveTicksRole = 1003;
}

BeakerItem::BeakerItem(QGraphicsItem* parent)
    : AbstractLiquidContainerItem(
        "beaker",
        "Beaker",
        ":/EzLaboratory/resources/image/glassware/beaker.png",
        QSizeF(100 * 1.5, 140 * 1.5),
        parent)
{
    setLiquidRenderingEnabled(true);

    m_pourHandle = new BeakerPourHandleItem(this);
    m_pourHandle->hide();
    m_pourHandle->setRatio(0.0);

    connect(m_pourHandle, &BeakerPourHandleItem::dragStarted, this, [this]() {
        m_trackVisible = true;
        m_pourHandle->setTrackVisible(true);
        update();
        });

    connect(m_pourHandle, &BeakerPourHandleItem::dragFinished, this, [this]() {
        m_trackVisible = false;
        m_pourHandle->setTrackVisible(false);
        update();
        });

    connect(m_pourHandle, &BeakerPourHandleItem::ratioChanged, this, [this](qreal ratio) {
        m_pourRatio = qBound(0.0, ratio, 1.0);
        refreshAttachedBottleTransform();
        update();
        });

    refreshHandleGeometry();

    m_scanTimerId = startTimer(50);
}

BeakerItem::~BeakerItem()
{
}

QRectF BeakerItem::liquidRectLocal() const
{
    return QRectF(18.0, 36.0, itemSize().width() - 36.0, itemSize().height() - 52.0);
}

QColor BeakerItem::defaultLiquidColor() const
{
    return QColor(90, 150, 255, 135);
}

qreal BeakerItem::defaultLiquidFillRatio() const
{
    return 0.50;
}

QPainterPath BeakerItem::liquidClipPathLocal() const
{
    const QRectF r = liquidRectLocal();

    QPainterPath path;

    const qreal leftTopInset = 6.0;
    const qreal rightTopInset = -8.0;
    const qreal leftBottomInset = 8.0;
    const qreal rightBottomInset = -8.0;

    path.moveTo(r.left() + leftTopInset, r.top());
    path.lineTo(r.right() - rightTopInset, r.top());
    path.lineTo(r.right() - rightBottomInset, r.bottom());
    path.lineTo(r.left() + leftBottomInset, r.bottom());
    path.closeSubpath();

    return path;
}

QVariant BeakerItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    const QVariant result = AbstractLiquidContainerItem::itemChange(change, value);

    if (change == QGraphicsItem::ItemPositionHasChanged) {
        if (m_attachedBottle) {
            detachBottle();
        }
        refreshHandleGeometry();
    }
    else if (change == QGraphicsItem::ItemRotationHasChanged) {
        refreshAttachedBottleTransform();
        refreshHandleGeometry();
    }

    return result;
}

void BeakerItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    AbstractLiquidContainerItem::paint(painter, option, widget);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (m_trackVisible && m_attachedBottle && m_pourHandle) {
        const QPointF a = QPointF(m_pourHandle->pos().x(), beakerAttachPivotLocalPos().y() + 10.0);
        const QPointF b = QPointF(m_pourHandle->pos().x(), itemSize().height() - 12.0);

        const QRectF lr = liquidRectLocal();
        const qreal fill = qBound(0.0, defaultLiquidFillRatio() + previewPourRatio(), 1.0);
        const qreal waterY = lr.bottom() - lr.height() * fill;

        painter->setPen(QPen(QColor(70, 110, 220), 1.5, Qt::DashLine));
        painter->drawLine(QPointF(lr.left() + 4.0, waterY), QPointF(lr.right() - 4.0, waterY));
    }

    painter->restore();
}

void BeakerItem::setPourHandleIconPath(const QString& iconPath)
{
    if (m_pourHandle) {
        m_pourHandle->setIconPath(iconPath);
    }
}

void BeakerItem::updateAttachmentScan()
{
    if (!scene()) {
        return;
    }

    const QPointF pivotScene = mapToScene(beakerAttachPivotLocalPos());

    bool beakerMovedRecently = false;
    if (m_hasLastSceneAnchorPos) {
        beakerMovedRecently = QLineF(m_lastSceneAnchorPos, pivotScene).length() > 1.0;
    }
    m_lastSceneAnchorPos = pivotScene;
    m_hasLastSceneAnchorPos = true;

    // 1. 已处于倒液弱绑定状态时，持续检查是否需要自动解除
    if (m_attachedBottle) {
        if (m_attachedBottle->hasPlug()) {
            detachBottle();
            return;
        }

        const qreal d = QLineF(m_attachedBottle->mouthScenePos(), pivotScene).length();

        // 弱绑定：偏离接触点一定距离就自动解除
        if (d > 26.0) {
            detachBottle();
            return;
        }

        // 仍然有效时，持续刷新位置
        refreshAttachedBottleTransform();
        return;
    }

    // 2. 解除后，只有当那只刚脱离的瓶子离开一定范围，才允许重新吸附
    if (m_detachSuppressedBottle) {
        if (!m_detachSuppressedBottle->scene()) {
            m_detachSuppressedBottle = nullptr;
        }
        else {
            const qreal d = QLineF(m_detachSuppressedBottle->mouthScenePos(), pivotScene).length();
            if (d < m_reacquireDistance) {
                return;
            }
            m_detachSuppressedBottle = nullptr;
        }
    }

    if (beakerMovedRecently) {
        return;
    }

    // 3. 空闲状态下再去扫描可吸附细口瓶
    const QRectF searchRect = mapRectToScene(boundingRect()).adjusted(-70.0, -30.0, 100.0, 40.0);
    const QList<QGraphicsItem*> items = scene()->items(searchRect, Qt::IntersectsItemBoundingRect);

    NarrowBottleItem* bestBottle = nullptr;
    qreal bestDistance = std::numeric_limits<qreal>::max();

    for (QGraphicsItem* item : items) {
        auto* bottle = dynamic_cast<NarrowBottleItem*>(item);
        if (!bottle) {
            continue;
        }

        if (bottle->data(kBottleAttachedToBeakerRole).toBool()) {
            continue;
        }

        if (bottle->hasPlug()) {
            continue;
        }

        if (bottle == m_detachSuppressedBottle) {
            continue;
        }

        const QPointF bottleMouthScene = bottle->mouthScenePos();

        bool movedRecently = false;
        const QVariant lastPosVar = bottle->data(kBottleLastMouthScenePosRole);
        int recentMoveTicks = bottle->data(kBottleRecentMoveTicksRole).toInt();

        if (lastPosVar.isValid()) {
            const qreal moveDist = QLineF(lastPosVar.toPointF(), bottleMouthScene).length();
            if (moveDist > 1.0) {
                recentMoveTicks = 4;
            }
            else if (recentMoveTicks > 0) {
                --recentMoveTicks;
            }
        }

        bottle->setData(kBottleLastMouthScenePosRole, bottleMouthScene);
        bottle->setData(kBottleRecentMoveTicksRole, recentMoveTicks);

        movedRecently = recentMoveTicks > 0;
        if (!movedRecently) {
            continue;
        }

        const qreal d = QLineF(bottleMouthScene, pivotScene).length();
        if (d < 70.0 && d < bestDistance) {
            bestDistance = d;
            bestBottle = bottle;
        }
    }

    if (bestBottle) {
        attachBottle(bestBottle);
    }
}

void BeakerItem::attachBottle(NarrowBottleItem* bottle)
{
    if (!scene()) {
        return;
    }

    if (!bottle || m_attachedBottle == bottle) {
        return;
    }


    m_attachedBottle = bottle;
    m_detachSuppressedBottle = nullptr;
    m_attachedBottle->setData(kBottleAttachedToBeakerRole, true);
    m_attachedBottle->setLabelOffset(QPointF(50.0, 0.0));
    m_attachedBottle->setTransformOriginPoint(m_attachedBottle->pourPivotLocalPos());
    m_attachedBottle->setZValue(this->zValue() + 10.0);

    m_pourRatio = 0.0;
    if (m_pourHandle) {
        m_pourHandle->setRatio(0.0);
        m_pourHandle->show();
    }

    refreshHandleGeometry();
    refreshAttachedBottleTransform();
    update();
}

void BeakerItem::detachBottle()
{
    if (!m_attachedBottle) {
        return;
    }

    m_attachedBottle->setData(kBottleAttachedToBeakerRole, false);
    m_attachedBottle->setLabelOffset(QPointF(0.0, 0.0));
    m_attachedBottle->setRotation(0.0);
    m_attachedBottle->setZValue(0.0);

    m_detachSuppressedBottle = m_attachedBottle;
    m_attachedBottle = nullptr;

    m_pourRatio = 0.0;

    if (m_pourHandle) {
        m_pourHandle->setRatio(0.0);
        m_trackVisible = false;
        m_pourHandle->setTrackVisible(false);
        m_pourHandle->hide();
    }

    update();
}

void BeakerItem::refreshAttachedBottleTransform()
{
    if (!m_attachedBottle) {
        return;
    }

    const QPointF pivotLocal = beakerAttachPivotLocalPos();
    const QPointF pivotScene = mapToScene(pivotLocal);
    const QPointF bottlePivotLocal = m_attachedBottle->pourPivotLocalPos();

    m_attachedBottle->setPos(pivotScene - bottlePivotLocal);

    const qreal angle = -maxPourRotationDeg() * m_pourRatio;
    m_attachedBottle->setRotation(angle);
}

void BeakerItem::refreshHandleGeometry()
{
    if (!m_pourHandle) {
        return;
    }

    const qreal x = itemSize().width() + 50.0;
    const qreal topY = beakerAttachPivotLocalPos().y() + 6.0;
    const qreal bottomY = itemSize().height() - 12.0;
    m_pourHandle->setTrackGeometry(x, topY, bottomY);
}

QPointF BeakerItem::beakerAttachPivotLocalPos() const
{
    const QRectF lr = liquidRectLocal();
    return QPointF(lr.right() - 2.0, lr.top() + 10.0);
}

qreal BeakerItem::maxPourRotationDeg() const
{
    return 78.0;
}

qreal BeakerItem::previewPourRatio() const
{
    return qMin(0.5, 0.5 * m_pourRatio);
}

void BeakerItem::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == m_scanTimerId) {
        updateAttachmentScan();
        return;
    }

    AbstractLiquidContainerItem::timerEvent(event);
}
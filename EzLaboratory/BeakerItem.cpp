#include "BeakerItem.h"
#include "NarrowBottleItem.h"
#include "BeakerPourHandleItem.h"
#include "SolidScatterItem.h"
#include <QColor>
#include <QDebug>

#include <QPainterPath>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QTimerEvent>

namespace
{
    constexpr int kBottleAttachedToBeakerRole = 1001;
}

BeakerItem::BeakerItem(QGraphicsItem* parent)
    : AbstractLiquidContainerItem(
        "beaker",
        "Beaker",
        ":/EzLaboratory/resources/image/glassware/beaker.png",
        QSizeF(100 * 1.5, 140 * 1.5),
        parent)
{
    m_currentLiquidLevel = 0.0;
    setLiquidLevel(0.0);
    setLiquidRenderingEnabled(false);

    // 按钮做成顶层图元，不挂在烧杯下面，保证永远在最上方
    m_pourHandle = new BeakerPourHandleItem();
    m_pourHandle->hide();
    m_pourHandle->setRatio(0.0);
    m_pourHandle->setZValue(10000.0);

    connect(m_pourHandle, &BeakerPourHandleItem::dragStarted, this, [this]() {
        if (m_attachedBottle && !liquidRenderingEnabled()) {
            setLiquidRenderingEnabled(true);
            setLiquidColor(m_attachedBottle->liquidColor());
            setLiquidLevel(m_currentLiquidLevel);
        }

        m_trackVisible = true;
        m_pourHandle->setTrackVisible(true);
        update();
        });

    connect(m_pourHandle, &BeakerPourHandleItem::dragFinished, this, [this]() {
        // 把本次倾倒预览量真正加到烧杯液位里
        m_currentLiquidLevel = qBound(0.0, m_currentLiquidLevel + previewPourRatio(), 1.0);
        setLiquidLevel(m_currentLiquidLevel);

        // 把附着瓶中的 chemical ids 合并进烧杯
        if (m_attachedBottle) {
            const QStringList ids = m_attachedBottle->containedChemicalIds();
            for (const QString& id : ids) {
                addContainedChemicalId(id);
            }
        }
        qDebug() << "[Beaker][dragFinished] attached bottle ids ="
            << (m_attachedBottle ? m_attachedBottle->containedChemicalIds() : QStringList{});
        qDebug() << "[Beaker][dragFinished] beaker ids =" << containedChemicalIds();
        // 结束预览状态
        m_trackVisible = false;
        m_pourHandle->setTrackVisible(false);

        // 按钮回到底部
        m_pourHandle->setRatio(0.0);

        // 瓶子回正
        m_pourRatio = 0.0;
        refreshAttachedBottleTransform();

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
    destroySolid();

    if (m_pourHandle) {
        delete m_pourHandle;
        m_pourHandle = nullptr;
    }
}

BeakerItem* BeakerItem::createInstance(bool enableLiquid,
    const QColor& liquidColor,
    QGraphicsItem* parent)
{
    auto* beaker = new BeakerItem(parent);

    if (enableLiquid) {
        beaker->m_currentLiquidLevel = beaker->defaultLiquidFillRatio();
        beaker->setLiquidLevel(beaker->m_currentLiquidLevel);
        beaker->setLiquidRenderingEnabled(true);

        if (liquidColor.isValid()) {
            beaker->setLiquidColor(liquidColor);
        }
    }
    else {
        beaker->m_currentLiquidLevel = 0.0;
        beaker->setLiquidLevel(0.0);
        beaker->setLiquidRenderingEnabled(false);
    }

    return beaker;
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
        // 烧杯一移动就自动解除状态，绝不带着瓶子走
        if (m_attachedBottle) {
            detachBottle();
        }
        refreshHandleGeometry();
    }
    else if (change == QGraphicsItem::ItemRotationHasChanged) {
        if (m_attachedBottle) {
            detachBottle();
        }
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

    // 这里只保留烧杯内部水位虚线
    if (m_trackVisible && m_attachedBottle) {
        const QRectF lr = liquidRectLocal();
        const qreal fill = qBound(0.0, m_currentLiquidLevel + previewPourRatio(), 1.0);
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

    // 已经处于倒液状态时，只做弱绑定解除判断
    if (m_attachedBottle) {
        if (m_attachedBottle->hasPlug()) {
            detachBottle();
            return;
        }

        const QPointF pivotScene = mapToScene(beakerAttachPivotLocalPos());
        const qreal d = QLineF(m_attachedBottle->mouthScenePos(), pivotScene).length();

        if (d > 30.0) {
            detachBottle();
            return;
        }

        refreshAttachedBottleTransform();
        return;
    }

    // 区域吸附：只在烧杯右侧区域内找“正在被用户拖动的无塞细口瓶”
    const QRectF zone = attachZoneSceneRect();
    const QList<QGraphicsItem*> items = scene()->items(zone, Qt::IntersectsItemBoundingRect);

    NarrowBottleItem* bestBottle = nullptr;
    qreal bestScore = std::numeric_limits<qreal>::max();

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

        // 关键：只有“瓶子在动/被拖”时才允许吸附，烧杯主动靠过去不会触发
        if (!bottle->isUnderMouse()) {
            continue;
        }

        const qreal score = QLineF(
            bottle->sceneBoundingRect().center(),
            zone.center()).length();

        if (score < bestScore) {
            bestScore = score;
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

    if (bottle->hasPlug()) {
        return;
    }

    m_attachedBottle = bottle;
    m_attachedBottle->setData(kBottleAttachedToBeakerRole, true);

    // 标签明显移向右侧，超出瓶身部分靠标签自己的裁剪逻辑处理
    m_attachedBottle->setLabelOffset(QPointF(50.0, 0.0));

    m_attachedBottle->setTransformOriginPoint(m_attachedBottle->pourPivotLocalPos());

    // 瓶子整体压到烧杯之上
    m_attachedBottle->setZValue(this->zValue() + 10.0);

    m_pourRatio = 0.0;

    if (m_pourHandle) {
        if (!m_pourHandle->scene() && scene()) {
            scene()->addItem(m_pourHandle);
        }
        m_pourHandle->setRatio(0.0);
        m_pourHandle->setTrackVisible(false);
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

    m_attachedBottle = nullptr;
    m_pourRatio = 0.0;
    m_trackVisible = false;

    if (m_pourHandle) {
        m_pourHandle->setRatio(0.0);
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

    const QPointF scenePivot = mapToScene(beakerAttachPivotLocalPos());

    // 想更往右，就继续调这个偏移
    const qreal x = scenePivot.x() + 92.0;
    const qreal topY = scenePivot.y() + 6.0;
    const qreal bottomY = mapToScene(QPointF(0.0, itemSize().height() - 12.0)).y();

    m_pourHandle->setTrackGeometry(x, topY, bottomY);
    m_pourHandle->setZValue(10000.0);
}

QRectF BeakerItem::attachZoneSceneRect() const
{
    const QRectF beakerSceneRect = mapRectToScene(boundingRect());

    // 这是吸附区：烧杯右侧一片区域
    // 想放宽/收紧触发区，就调这里
    return QRectF(
        beakerSceneRect.right() - 5.0,
        beakerSceneRect.top() + 8.0,
        8.0,
        beakerSceneRect.height() - 16.0
    );
}

QPointF BeakerItem::beakerAttachPivotLocalPos() const
{
    const QRectF lr = liquidRectLocal();

    // 这是烧杯侧吸附锚点，也是旋转接触点
    return QPointF(lr.right() + 20.0, lr.top() - 15.0);
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

void BeakerItem::acceptSolidFromTweezers(const QString& chemicalId, const QString& texturePath)
{
    if (chemicalId.isEmpty() || texturePath.isEmpty()) {
        return;
    }

    m_solidChemicalId = chemicalId;
    m_solidTexturePath = texturePath;
    m_solidFillRatio = 0.35;

    addContainedChemicalId(chemicalId);

    qDebug() << "[Beaker][acceptSolidFromTweezers] added =" << chemicalId;
    qDebug() << "[Beaker][acceptSolidFromTweezers] beaker ids =" << containedChemicalIds();

    ensureSolidCreated();
    if (m_solidItem) {
        m_solidItem->setTexturePath(m_solidTexturePath);
        m_solidItem->setFillRatio(m_solidFillRatio);
        refreshSolidGeometry();
    }
}

bool BeakerItem::hasSolidItem() const
{
    return m_solidItem != nullptr;
}

void BeakerItem::ensureSolidCreated()
{
    if (m_solidItem) {
        refreshSolidGeometry();
        return;
    }

    if (m_solidTexturePath.isEmpty() || m_solidFillRatio <= 0.0) {
        return;
    }

    m_solidItem = new SolidScatterItem(this);
    m_solidItem->setParentItem(this);
    m_solidItem->setTexturePath(m_solidTexturePath);
    m_solidItem->setFillRatio(m_solidFillRatio);
    m_solidItem->setZValue(0.25);
    refreshSolidGeometry();
}

void BeakerItem::destroySolid()
{
    if (!m_solidItem) {
        return;
    }

    delete m_solidItem;
    m_solidItem = nullptr;
}

void BeakerItem::refreshSolidGeometry()
{
    if (!m_solidItem) {
        return;
    }

    m_solidItem->setContainerRect(liquidRectLocal());
    m_solidItem->setClipPath(liquidClipPathLocal());
}
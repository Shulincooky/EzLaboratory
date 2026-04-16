#include "EzLaboratory.h"

#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QScrollBar>
#include <QTimer>

EzLaboratory::EzLaboratory(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::EzLaboratoryClass())
{
    ui->setupUi(this);
    initLabScene();

    QTimer::singleShot(0, this, [this]() {
        updateViewAfterResize();
        });
}

EzLaboratory::~EzLaboratory()
{
    delete ui;
}

void EzLaboratory::initLabScene()
{
    m_scene = new QGraphicsScene(this);

    // 场景边界固定为一个更大的“实验世界”
    m_scene->setSceneRect(m_worldRect);

    ui->graphicsViewLab->setScene(m_scene);
    ui->graphicsViewLab->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsViewLab->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->graphicsViewLab->setBackgroundBrush(QColor(245, 245, 245));

    // 允许滚动条存在，场景拖动本质上就是在改变视图滚动位置
    ui->graphicsViewLab->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->graphicsViewLab->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 监听 viewport 尺寸变化
    ui->graphicsViewLab->viewport()->installEventFilter(this);

    // 画一个大实验台背景，方便看边界
    //m_scene->addRect(
    //    m_worldRect,
    //    QPen(QColor(160, 160, 160), 2),
    //    QBrush(QColor(235, 235, 235))
    //);

    // 放一个基础可拖拽物体
    m_testItem = new QGraphicsRectItem(0, 0, 100, 140);
    m_testItem->setPen(QPen(QColor(50, 90, 180), 2));
    m_testItem->setBrush(QBrush(QColor(180, 220, 255)));
    m_testItem->setPos(200, 200);

    m_testItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    m_testItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_testItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    m_scene->addItem(m_testItem);

    clampItemIntoScene();

    // 初始时把视图中心放到世界中心附近
    ui->graphicsViewLab->centerOn(m_worldRect.center());
}

bool EzLaboratory::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->graphicsViewLab->viewport() && event->type() == QEvent::Resize) {
        updateViewAfterResize();
    }

    return QMainWindow::eventFilter(watched, event);
}

void EzLaboratory::updateViewAfterResize()
{
    if (!ui || !ui->graphicsViewLab || !m_scene)
        return;

    // 场景边界固定，不再随着窗口大小改变
    m_scene->setSceneRect(m_worldRect);

    // 确保当前滚动位置不超界
    QScrollBar* hBar = ui->graphicsViewLab->horizontalScrollBar();
    QScrollBar* vBar = ui->graphicsViewLab->verticalScrollBar();

    if (hBar) {
        hBar->setValue(qBound(hBar->minimum(), hBar->value(), hBar->maximum()));
    }
    if (vBar) {
        vBar->setValue(qBound(vBar->minimum(), vBar->value(), vBar->maximum()));
    }

    clampItemIntoScene();
}

void EzLaboratory::clampItemIntoScene()
{
    if (!m_scene || !m_testItem)
        return;

    QRectF sceneRect = m_scene->sceneRect();
    QRectF itemRect = m_testItem->sceneBoundingRect();

    qreal newX = m_testItem->pos().x();
    qreal newY = m_testItem->pos().y();

    if (itemRect.left() < sceneRect.left())
        newX += sceneRect.left() - itemRect.left();
    if (itemRect.top() < sceneRect.top())
        newY += sceneRect.top() - itemRect.top();
    if (itemRect.right() > sceneRect.right())
        newX -= itemRect.right() - sceneRect.right();
    if (itemRect.bottom() > sceneRect.bottom())
        newY -= itemRect.bottom() - sceneRect.bottom();

    m_testItem->setPos(newX, newY);
}
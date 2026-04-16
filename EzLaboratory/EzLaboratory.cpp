#include "EzLaboratory.h"

#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QSize>

EzLaboratory::EzLaboratory(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::EzLaboratoryClass())
{
    ui->setupUi(this);
    initLabScene();
}

EzLaboratory::~EzLaboratory()
{
    delete ui;
}

void EzLaboratory::initLabScene()
{
    // 1. 创建场景
    m_scene = new QGraphicsScene(this);

    // 2. 绑定到你在 .ui 里已经放好的 QGraphicsView
    ui->graphicsViewLab->setScene(m_scene);
    ui->graphicsViewLab->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsViewLab->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->graphicsViewLab->setDragMode(QGraphicsView::NoDrag);

    // 固定滚动条策略，避免后续如果用了 fitInView 时触发不必要的问题
    ui->graphicsViewLab->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewLab->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->graphicsViewLab->setBackgroundBrush(QColor(245, 245, 245));

    // 3. 先让 scene 大小和 view 可视区域一致
    updateSceneSize();

    // 4. 放一个基础可拖拽物体
    m_testItem = new QGraphicsRectItem(0, 0, 100, 140);
    m_testItem->setPen(QPen(QColor(50, 90, 180), 2));
    m_testItem->setBrush(QBrush(QColor(180, 220, 255)));

    // 初始放在场景中偏左的位置
    m_testItem->setPos(80, 80);

    m_testItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    m_testItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_testItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    m_scene->addItem(m_testItem);
}

void EzLaboratory::updateSceneSize()
{
    if (!ui || !ui->graphicsViewLab || !m_scene)
        return;

    const QSize viewSize = ui->graphicsViewLab->viewport()->size();

    // 让 sceneRect 始终等于当前 QGraphicsView 的可视区域大小
    m_scene->setSceneRect(0, 0, viewSize.width(), viewSize.height());
}

void EzLaboratory::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    updateSceneSize();
}
#include "EzLaboratory.h"
#include "BeakerItem.h"
#include "LabwareItemDelegate.h"

#include <QScrollBar>
#include <QTimer>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QIcon>

namespace
{
    constexpr int LabwareTypeRole = Qt::UserRole + 1;
    constexpr int LabwareLimitRole = Qt::UserRole + 2;
    constexpr int LabwareRemainingRole = Qt::UserRole + 3;
}

EzLaboratory::EzLaboratory(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::EzLaboratoryClass())
{
    ui->setupUi(this);
    initLabwareList();
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
    connect(ui->graphicsViewLab, &LabGraphicsView::labwareDropped,
        this, &EzLaboratory::decreaseRemainingCount);

    ui->graphicsViewLab->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsViewLab->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    // 允许滚动条存在，场景拖动本质上就是在改变视图滚动位置
    ui->graphicsViewLab->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewLab->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 监听 viewport 尺寸变化
    ui->graphicsViewLab->viewport()->installEventFilter(this);

    // 放一个基础可拖拽物体
    m_testItem = new BeakerItem();
    m_testItem->setPos(200, 200);
    m_scene->addItem(m_testItem);
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

}
void EzLaboratory::initLabwareList()
{
    m_labwareModel = new QStandardItemModel(this);
    ui->labwareList->setModel(m_labwareModel);
    ui->labwareList->setItemDelegate(new LabwareItemDelegate(ui->labwareList));
    ui->labwareList->setMouseTracking(true);
    ui->labwareList->setSpacing(6);

    ui->labwareList->setUniformItemSizes(true);
    ui->labwareList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->labwareList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->labwareList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* beakerItem = new QStandardItem(QIcon(":/EzLaboratory/resources/image/glassware/beaker.png"), "烧杯");
    beakerItem->setEditable(false);
    beakerItem->setData("beaker", LabwareTypeRole);
    beakerItem->setData(3, LabwareLimitRole);
    beakerItem->setData(3, LabwareRemainingRole);  // 这里限制最多 3 个

    m_labwareModel->appendRow(beakerItem);

    // 如果你想“列表里视觉上出现两个相同器具”，再加一份：
    // auto* beakerItem2 = new QStandardItem(QIcon(":/EzLaboratory/resources/image/glassware/beaker.png"), "烧杯");
    // beakerItem2->setEditable(false);
    // beakerItem2->setData("beaker", LabwareTypeRole);
    // beakerItem2->setData(3, LabwareLimitRole);
    // m_labwareModel->appendRow(beakerItem2);
}
void EzLaboratory::decreaseRemainingCount(const QString& type)
{
    if (!m_labwareModel)
        return;

    for (int row = 0; row < m_labwareModel->rowCount(); ++row) {
        QStandardItem* item = m_labwareModel->item(row);
        if (!item)
            continue;

        if (item->data(LabwareTypeRole).toString() != type)
            continue;

        const int limit = item->data(LabwareLimitRole).toInt();
        if (limit <= 0)
            return;

        int remaining = item->data(LabwareRemainingRole).toInt();
        remaining = qMax(0, remaining - 1);
        item->setData(remaining, LabwareRemainingRole);

        // 触发视图刷新
        QModelIndex idx = m_labwareModel->indexFromItem(item);
        emit m_labwareModel->dataChanged(idx, idx);

        return;
    }
}
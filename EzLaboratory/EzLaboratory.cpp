#include "EzLaboratory.h"
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

    m_scene->setSceneRect(m_worldRect);

    ui->graphicsViewLab->setScene(m_scene);
    connect(ui->graphicsViewLab, &LabGraphicsView::labwareDropped,
        this, &EzLaboratory::decreaseRemainingCount);

    ui->graphicsViewLab->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsViewLab->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    ui->graphicsViewLab->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewLab->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->graphicsViewLab->viewport()->installEventFilter(this);
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

    m_scene->setSceneRect(m_worldRect);

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
    auto* delegate = new LabwareItemDelegate(ui->labwareList);
    delegate->setViewWidth(ui->labwareList->viewport()->width());
    ui->labwareList->setItemDelegate(delegate);

    ui->labwareList->setMouseTracking(true);
    ui->labwareList->setSpacing(6);

    ui->labwareList->setResizeMode(QListView::Adjust);
    ui->labwareList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->labwareList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->labwareList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* beakerItem = new QStandardItem(QIcon(":/EzLaboratory/resources/image/glassware/beaker.png"), "烧杯");
    beakerItem->setEditable(false);
    beakerItem->setData("beaker", LabwareTypeRole);
    beakerItem->setData(3, LabwareLimitRole);
    beakerItem->setData(3, LabwareRemainingRole);
    m_labwareModel->appendRow(beakerItem);

    auto* narrowBottleItem = new QStandardItem(
        QIcon(":/EzLaboratory/resources/image/glassware/generic/narrow-mouth_bottle/bottle.png"),
        "细口瓶");
    narrowBottleItem->setEditable(false);
    narrowBottleItem->setData("narrow_bottle", LabwareTypeRole);
    narrowBottleItem->setData(3, LabwareLimitRole);
    narrowBottleItem->setData(3, LabwareRemainingRole);
    m_labwareModel->appendRow(narrowBottleItem);

    auto* wideBottleItem = new QStandardItem(
        QIcon(":/EzLaboratory/resources/image/glassware/generic/wide-mouth_bottle/bottle.png"),
        "广口瓶");
    wideBottleItem->setEditable(false);
    wideBottleItem->setData("wide_bottle", LabwareTypeRole);
    wideBottleItem->setData(3, LabwareLimitRole);
    wideBottleItem->setData(3, LabwareRemainingRole);
    m_labwareModel->appendRow(wideBottleItem);
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

        QModelIndex idx = m_labwareModel->indexFromItem(item);
        emit m_labwareModel->dataChanged(idx, idx);

        return;
    }
}
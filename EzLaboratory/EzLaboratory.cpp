#include "EzLaboratory.h"

#include "BottleLabelData.h"
#include "LabwareItemDelegate.h"
#include "LabwareTemplateRoles.h"
#include "NarrowBottleItem.h"
#include "WideBottleItem.h"

#include <QEvent>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>

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

    appendCommonContainerItem(
        QStringLiteral("beaker"),
        QStringLiteral("烧杯"),
        QStringLiteral(":/EzLaboratory/resources/image/glassware/beaker.png"),
        false);

    appendNarrowBottleInstanceItem(
        QStringLiteral("盐酸细口瓶"),
        QStringLiteral("HCl"),
        QStringLiteral("4 mol/L"),
        true,
        QColor(25, 15, 255, 125));

    appendNarrowBottleInstanceItem(
        QStringLiteral("蒸馏水细口瓶"),
        QStringLiteral("蒸馏水"),
        true,
        QColor(90, 150, 255, 80));

    appendWideBottleInstanceItem(
        QStringLiteral("氢氧化钠广口瓶"),
        QStringLiteral("氢氧化钠"),
        false,
        QColor(),
        true,
        QStringLiteral(":/EzLaboratory/resources/image/solid/naoh_chunk.png"),
        0.65);

    appendWideBottleInstanceItem(
        QStringLiteral("空广口瓶"),
        QStringLiteral("空瓶"),
        false,
        QColor(),
        false,
        QString(),
        0.0);
}

void EzLaboratory::decreaseRemainingCount(const QString& templateId)
{
    if (!m_labwareModel)
        return;

    for (int row = 0; row < m_labwareModel->rowCount(); ++row) {
        QStandardItem* item = m_labwareModel->item(row);
        if (!item)
            continue;

        if (item->data(LabwareRoles::TemplateId).toString() != templateId)
            continue;

        const int limit = item->data(LabwareRoles::Limit).toInt();
        if (limit <= 0)
            return;

        int remaining = item->data(LabwareRoles::Remaining).toInt();
        remaining = qMax(0, remaining - 1);
        item->setData(remaining, LabwareRoles::Remaining);

        QModelIndex idx = m_labwareModel->indexFromItem(item);
        emit m_labwareModel->dataChanged(idx, idx);
        return;
    }
}

void EzLaboratory::appendNarrowBottleInstanceItem(const QString& displayName,
    const QString& centerText,
    bool enableLiquid,
    const QColor& liquidColor,
    int limit)
{
    if (!m_labwareModel)
        return;

    auto* item = new QStandardItem(
        QIcon(":/EzLaboratory/resources/image/glassware/generic/narrow-mouth_bottle/bottle.png"),
        displayName);

    item->setEditable(false);
    item->setData(QStringLiteral("narrow_bottle"), LabwareRoles::Type);
    item->setData(limit, LabwareRoles::Limit);
    item->setData(limit, LabwareRoles::Remaining);
    item->setData(QStringLiteral("narrow_bottle::") + displayName, LabwareRoles::TemplateId);

    item->setData(static_cast<int>(BottleLabelLayout::SingleCenter), LabwareRoles::LabelLayout);
    item->setData(centerText, LabwareRoles::LabelCenterText);
    item->setData(QString(), LabwareRoles::LabelTopText);
    item->setData(QString(), LabwareRoles::LabelBottomText);

    item->setData(enableLiquid, LabwareRoles::LiquidEnabled);
    item->setData(liquidColor, LabwareRoles::LiquidColor);

    item->setData(false, LabwareRoles::SolidEnabled);
    item->setData(QString(), LabwareRoles::SolidTexturePath);
    item->setData(0.0, LabwareRoles::SolidFillRatio);

    m_labwareModel->appendRow(item);
}

void EzLaboratory::appendNarrowBottleInstanceItem(const QString& displayName,
    const QString& topText,
    const QString& bottomText,
    bool enableLiquid,
    const QColor& liquidColor,
    int limit)
{
    if (!m_labwareModel)
        return;

    auto* item = new QStandardItem(
        QIcon(":/EzLaboratory/resources/image/glassware/generic/narrow-mouth_bottle/bottle.png"),
        displayName);

    item->setEditable(false);
    item->setData(QStringLiteral("narrow_bottle"), LabwareRoles::Type);
    item->setData(limit, LabwareRoles::Limit);
    item->setData(limit, LabwareRoles::Remaining);
    item->setData(QStringLiteral("narrow_bottle::") + displayName, LabwareRoles::TemplateId);

    item->setData(static_cast<int>(BottleLabelLayout::DoubleLine), LabwareRoles::LabelLayout);
    item->setData(QString(), LabwareRoles::LabelCenterText);
    item->setData(topText, LabwareRoles::LabelTopText);
    item->setData(bottomText, LabwareRoles::LabelBottomText);

    item->setData(enableLiquid, LabwareRoles::LiquidEnabled);
    item->setData(liquidColor, LabwareRoles::LiquidColor);

    item->setData(false, LabwareRoles::SolidEnabled);
    item->setData(QString(), LabwareRoles::SolidTexturePath);
    item->setData(0.0, LabwareRoles::SolidFillRatio);

    m_labwareModel->appendRow(item);
}

void EzLaboratory::appendWideBottleInstanceItem(const QString& displayName,
    const QString& centerText,
    bool enableLiquid,
    const QColor& liquidColor,
    bool enableSolid,
    const QString& solidTexturePath,
    qreal solidFillRatio,
    int limit)
{
    if (!m_labwareModel)
        return;

    auto* item = new QStandardItem(
        QIcon(":/EzLaboratory/resources/image/glassware/generic/wide-mouth_bottle/bottle.png"),
        displayName);

    item->setEditable(false);
    item->setData(QStringLiteral("wide_bottle"), LabwareRoles::Type);
    item->setData(limit, LabwareRoles::Limit);
    item->setData(limit, LabwareRoles::Remaining);
    item->setData(QStringLiteral("wide_bottle::") + displayName, LabwareRoles::TemplateId);

    item->setData(static_cast<int>(BottleLabelLayout::SingleCenter), LabwareRoles::LabelLayout);
    item->setData(centerText, LabwareRoles::LabelCenterText);
    item->setData(QString(), LabwareRoles::LabelTopText);
    item->setData(QString(), LabwareRoles::LabelBottomText);

    item->setData(enableLiquid, LabwareRoles::LiquidEnabled);
    item->setData(liquidColor, LabwareRoles::LiquidColor);

    item->setData(enableSolid, LabwareRoles::SolidEnabled);
    item->setData(solidTexturePath, LabwareRoles::SolidTexturePath);
    item->setData(solidFillRatio, LabwareRoles::SolidFillRatio);

    m_labwareModel->appendRow(item);
}

void EzLaboratory::appendWideBottleInstanceItem(const QString& displayName,
    const QString& topText,
    const QString& bottomText,
    bool enableLiquid,
    const QColor& liquidColor,
    bool enableSolid,
    const QString& solidTexturePath,
    qreal solidFillRatio,
    int limit)
{
    if (!m_labwareModel)
        return;

    auto* item = new QStandardItem(
        QIcon(":/EzLaboratory/resources/image/glassware/generic/wide-mouth_bottle/bottle.png"),
        displayName);

    item->setEditable(false);
    item->setData(QStringLiteral("wide_bottle"), LabwareRoles::Type);
    item->setData(limit, LabwareRoles::Limit);
    item->setData(limit, LabwareRoles::Remaining);
    item->setData(QStringLiteral("wide_bottle::") + displayName, LabwareRoles::TemplateId);

    item->setData(static_cast<int>(BottleLabelLayout::DoubleLine), LabwareRoles::LabelLayout);
    item->setData(QString(), LabwareRoles::LabelCenterText);
    item->setData(topText, LabwareRoles::LabelTopText);
    item->setData(bottomText, LabwareRoles::LabelBottomText);

    item->setData(enableLiquid, LabwareRoles::LiquidEnabled);
    item->setData(liquidColor, LabwareRoles::LiquidColor);

    item->setData(enableSolid, LabwareRoles::SolidEnabled);
    item->setData(solidTexturePath, LabwareRoles::SolidTexturePath);
    item->setData(solidFillRatio, LabwareRoles::SolidFillRatio);

    m_labwareModel->appendRow(item);
}

void EzLaboratory::appendCommonContainerItem(const QString& type,
    const QString& displayName,
    const QString& iconPath,
    bool enableLiquid,
    const QColor& liquidColor,
    int limit)
{
    if (!m_labwareModel)
        return;

    auto* item = new QStandardItem(QIcon(iconPath), displayName);
    item->setEditable(false);

    item->setData(type, LabwareRoles::Type);
    item->setData(limit, LabwareRoles::Limit);
    item->setData(limit, LabwareRoles::Remaining);
    item->setData(type + QStringLiteral("::") + displayName, LabwareRoles::TemplateId);

    item->setData(-1, LabwareRoles::LabelLayout);
    item->setData(QString(), LabwareRoles::LabelCenterText);
    item->setData(QString(), LabwareRoles::LabelTopText);
    item->setData(QString(), LabwareRoles::LabelBottomText);

    item->setData(enableLiquid, LabwareRoles::LiquidEnabled);
    item->setData(liquidColor, LabwareRoles::LiquidColor);

    item->setData(false, LabwareRoles::SolidEnabled);
    item->setData(QString(), LabwareRoles::SolidTexturePath);
    item->setData(0.0, LabwareRoles::SolidFillRatio);

    m_labwareModel->appendRow(item);
}
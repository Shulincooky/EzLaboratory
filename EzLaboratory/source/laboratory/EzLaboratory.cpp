#include "EzLaboratory.h"

#include "BottleLabelData.h"
#include "ExperimentParser.h"
#include "LabwareItemDelegate.h"
#include "LabwareTemplateRoles.h"
#include "NarrowBottleItem.h"
#include "WideBottleItem.h"
#include "BeakerItem.h"
#include "ui_EzLaboratory.h"

#include <QEvent>
#include <QGraphicsScene>
#include <QIcon>
#include <QScrollBar>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>

namespace
{
    BottleLabelLayout labelLayoutFromText(const QString& layout)
    {
        if (layout.compare(QStringLiteral("double"), Qt::CaseInsensitive) == 0) {
            return BottleLabelLayout::DoubleLine;
        }

        return BottleLabelLayout::SingleCenter;
    }

    QIcon labwareIconFor(const QString& bodyType)
    {
        if (bodyType == QStringLiteral("beaker")) {
            return QIcon(QStringLiteral(":/EzLaboratory/images/labcontainer/beaker.png"));
        }
        if (bodyType == QStringLiteral("narrow_bottle")) {
            return QIcon(QStringLiteral(":/EzLaboratory/images/labcontainer/narrow-neck_bottle/bottle.png"));
        }
        if (bodyType == QStringLiteral("wide_bottle")) {
            return QIcon(QStringLiteral(":/EzLaboratory/images/labcontainer/wide-mouth_bottle/bottle.png"));
        }

        return {};
    }

    QColor initialLiquidColorFor(const ExperimentLabwareInfo& labware,
        const ExperimentDefinition& experiment)
    {
        for (const QString& chemicalId : labware.chemicalIds) {
            const auto chemical = experiment.chemicals.constFind(chemicalId);
            if (chemical != experiment.chemicals.cend()
                && chemical.value().appearance.color.isValid()) {
                return chemical.value().appearance.color;
            }
        }

        return {};
    }
}

EzLaboratory::EzLaboratory(QWidget* parent)
    : QWidget(parent)
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

void EzLaboratory::initializeExperiment(const ExperimentDefinition& experiment)
{
    setWindowTitle(experiment.card.title.isEmpty()
        ? QStringLiteral("EzLaboratory")
        : experiment.card.title);

    if (m_labwareModel) {
        m_labwareModel->clear();
    }

    if (m_scene) {
        m_scene->clear();
        m_scene->setSceneRect(m_worldRect);
    }

    for (const ExperimentLabwareInfo& labware : experiment.labwares) {
        addLabwareTemplate(labware, experiment);
    }
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
    return QWidget::eventFilter(watched, event);
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
}

void EzLaboratory::addLabwareTemplate(const ExperimentLabwareInfo& labware,
    const ExperimentDefinition& experiment)
{
    if (!m_labwareModel || labware.bodyType.isEmpty()) {
        return;
    }

    auto* item = new QStandardItem(labwareIconFor(labware.bodyType), labware.name);
    item->setEditable(false);
    item->setData(labware.bodyType, LabwareRoles::Type);
    item->setData(labware.limit, LabwareRoles::Limit);
    item->setData(labware.limit, LabwareRoles::Remaining);
    item->setData(static_cast<int>(labelLayoutFromText(labware.label.layout)),
        LabwareRoles::LabelLayout);
    item->setData(labware.label.centerText, LabwareRoles::LabelCenterText);
    item->setData(labware.label.topText, LabwareRoles::LabelTopText);
    item->setData(labware.label.bottomText, LabwareRoles::LabelBottomText);
    item->setData(labware.id, LabwareRoles::TemplateId);
    item->setData(!labware.chemicalIds.isEmpty(), LabwareRoles::LiquidEnabled);
    item->setData(initialLiquidColorFor(labware, experiment), LabwareRoles::LiquidColor);
    item->setData(labware.chemicalIds, LabwareRoles::ChemicalIds);

    m_labwareModel->appendRow(item);
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


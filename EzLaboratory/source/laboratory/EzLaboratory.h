#pragma once

#include <QWidget>
#include <QGraphicsScene>
#include <QEvent>
#include <QColor>

QT_BEGIN_NAMESPACE
namespace Ui { class EzLaboratoryClass; };
QT_END_NAMESPACE
struct ExperimentDefinition;
struct ExperimentLabwareInfo;
class QStandardItemModel;

class EzLaboratory : public QWidget
{
    Q_OBJECT

public:
    EzLaboratory(QWidget* parent = nullptr);
    ~EzLaboratory();

    void initializeExperiment(const ExperimentDefinition& experiment);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    Ui::EzLaboratoryClass* ui;

    QStandardItemModel* m_labwareModel = nullptr;

    QGraphicsScene* m_scene = nullptr;

    QRectF m_worldRect = QRectF(0, 0, 2400, 1600);

    void initLabwareList();
    void initLabScene();
    void addLabwareTemplate(const ExperimentLabwareInfo& labware,
        const ExperimentDefinition& experiment);
    void updateViewAfterResize();
    void decreaseRemainingCount(const QString& templateId);
};

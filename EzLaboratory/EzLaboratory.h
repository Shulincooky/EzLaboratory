#pragma once

#include <QtWidgets/QMainWindow>
#include <QGraphicsScene>
#include <QEvent>
#include "ui_EzLaboratory.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EzLaboratoryClass; };
QT_END_NAMESPACE
class LabItem;

class EzLaboratory : public QMainWindow
{
    Q_OBJECT

public:
    EzLaboratory(QWidget* parent = nullptr);
    ~EzLaboratory();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    Ui::EzLaboratoryClass* ui;

    QGraphicsScene* m_scene = nullptr;
    LabItem* m_testItem = nullptr;

    // 固定的最大场景边界
    QRectF m_worldRect = QRectF(0, 0, 2400, 1600);

    void initLabScene();
    void updateViewAfterResize();
    void clampItemIntoScene();
};
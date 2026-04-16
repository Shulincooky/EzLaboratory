#pragma once

#include <QtWidgets/QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QResizeEvent>
#include "ui_EzLaboratory.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EzLaboratoryClass; };
QT_END_NAMESPACE

class EzLaboratory : public QMainWindow
{
    Q_OBJECT

public:
    EzLaboratory(QWidget* parent = nullptr);
    ~EzLaboratory();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::EzLaboratoryClass* ui;

    QGraphicsScene* m_scene = nullptr;
    QGraphicsRectItem* m_testItem = nullptr;

    void initLabScene();
    void updateSceneSize();
};
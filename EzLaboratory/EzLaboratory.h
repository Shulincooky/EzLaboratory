#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_EzLaboratory.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EzLaboratoryClass; };
QT_END_NAMESPACE

class EzLaboratory : public QMainWindow
{
    Q_OBJECT

public:
    EzLaboratory(QWidget *parent = nullptr);
    ~EzLaboratory();

private:
    Ui::EzLaboratoryClass *ui;
};


#include "EzLaboratory.h"

EzLaboratory::EzLaboratory(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EzLaboratoryClass())
{
    ui->setupUi(this);
}

EzLaboratory::~EzLaboratory()
{
    delete ui;
}


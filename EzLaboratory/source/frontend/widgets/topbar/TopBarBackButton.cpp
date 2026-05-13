#include "TopBarBackButton.h"

#include "ui_TopBarBackButton.h"

#include <QToolButton>

TopBarBackButton::TopBarBackButton(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TopBarBackButton())
{
    ui->setupUi(this);
    setFixedSize(76, 36);
    ui->backButton->setCursor(Qt::PointingHandCursor);

    connect(ui->backButton, &QToolButton::clicked, this, [this]() {
        emit clicked();
    });
}

TopBarBackButton::~TopBarBackButton()
{
    delete ui;
}

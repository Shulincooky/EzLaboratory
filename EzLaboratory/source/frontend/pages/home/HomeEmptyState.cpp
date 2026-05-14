#include "HomeEmptyState.h"

#include "ui_HomeEmptyState.h"

HomeEmptyState::HomeEmptyState(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HomeEmptyState())
{
    ui->setupUi(this);
}

HomeEmptyState::~HomeEmptyState()
{
    delete ui;
}

void HomeEmptyState::setContent(const QString& title, const QString& message)
{
    setTitle(title);
    setMessage(message);
}

void HomeEmptyState::setTitle(const QString& title)
{
    ui->titleLabel->setText(title);
}

void HomeEmptyState::setMessage(const QString& message)
{
    ui->messageLabel->setText(message);
}

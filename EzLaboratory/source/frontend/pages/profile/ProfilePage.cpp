#include "ProfilePage.h"

#include "ui_ProfilePage.h"

ProfilePage::ProfilePage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ProfilePage())
{
    ui->setupUi(this);
}

ProfilePage::~ProfilePage()
{
    delete ui;
}

void ProfilePage::setUserIdentity(const QString& email, const QString& nickname)
{
    ui->emailEdit->setText(email);
    ui->nicknameEdit->setText(nickname.isEmpty() ? email.section('@', 0, 0) : nickname);
}

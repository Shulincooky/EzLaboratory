#include "SettingsPage.h"

#include "ui_SettingsPage.h"

#include <QIcon>
#include <QLabel>
#include <QPushButton>

namespace
{
    constexpr const char* kDefaultAvatarPath = ":/Main/icons/sidebar/avatar_default.svg";

    void setSvgIconPixmap(QLabel* label, const QString& path)
    {
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignCenter);
        label->setPixmap(QIcon(path).pixmap(label->size(), label->devicePixelRatioF()));
    }
}

SettingsPage::SettingsPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingsPage())
{
    ui->setupUi(this);

    setSvgIconPixmap(ui->avatarLabel, QString::fromLatin1(kDefaultAvatarPath));

    connect(ui->editProfileButton, &QPushButton::clicked, this, [this]() {
        emit editProfileRequested();
    });
}

SettingsPage::~SettingsPage()
{
    delete ui;
}

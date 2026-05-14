#include "ProfilePage.h"

#include "ui_ProfilePage.h"

#include <QIcon>
#include <QLabel>

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

ProfilePage::ProfilePage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ProfilePage())
{
    ui->setupUi(this);
    setSvgIconPixmap(ui->avatarPreview, QString::fromLatin1(kDefaultAvatarPath));
    setUserIdentity(QStringLiteral("local@ezlaboratory.local"), QStringLiteral("Admin"));
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

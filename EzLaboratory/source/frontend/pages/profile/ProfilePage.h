#pragma once

#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ProfilePage; }
QT_END_NAMESPACE

class ProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePage(QWidget* parent = nullptr);
    ~ProfilePage() override;

    void setUserIdentity(const QString& email, const QString& nickname);

private:
    Ui::ProfilePage* ui = nullptr;
};

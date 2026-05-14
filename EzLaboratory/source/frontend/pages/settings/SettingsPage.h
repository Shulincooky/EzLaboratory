#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsPage; }
QT_END_NAMESPACE

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget* parent = nullptr);
    ~SettingsPage() override;

signals:
    void editProfileRequested();

private:
    Ui::SettingsPage* ui;
};

#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TopBarBackButton; }
QT_END_NAMESPACE

class TopBarBackButton : public QWidget
{
    Q_OBJECT

public:
    explicit TopBarBackButton(QWidget* parent = nullptr);
    ~TopBarBackButton() override;

signals:
    void clicked();

private:
    Ui::TopBarBackButton* ui;
};

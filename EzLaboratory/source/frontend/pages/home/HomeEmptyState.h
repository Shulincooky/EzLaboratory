#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class HomeEmptyState; }
QT_END_NAMESPACE

class HomeEmptyState : public QWidget
{
    Q_OBJECT

public:
    explicit HomeEmptyState(QWidget* parent = nullptr);
    ~HomeEmptyState() override;

    void setTitle(const QString& title);
    void setMessage(const QString& message);

private:
    Ui::HomeEmptyState* ui;
};

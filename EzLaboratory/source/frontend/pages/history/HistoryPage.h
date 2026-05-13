#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class HistoryPage; }
QT_END_NAMESPACE

class HistoryPage : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryPage(QWidget* parent = nullptr);
    ~HistoryPage() override;

private:
    Ui::HistoryPage* ui = nullptr;
};

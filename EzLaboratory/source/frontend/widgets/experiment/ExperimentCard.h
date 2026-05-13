#pragma once

#include <QObject>
#include <QString>
#include <QWidget>

class QMouseEvent;

QT_BEGIN_NAMESPACE
namespace Ui { class ExperimentCard; }
QT_END_NAMESPACE

class ExperimentCard : public QWidget
{
    Q_OBJECT

public:
    explicit ExperimentCard(QWidget* parent = nullptr);
    ~ExperimentCard() override;

    QString id() const;
    void setId(const QString& id);
    void setTitle(const QString& title);
    void setSummary(const QString& summary);
    void setLocal(bool local);

signals:
    void clicked(const QString& id);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    Ui::ExperimentCard* ui;
    QString m_id;
};

#pragma once

#include <QObject>
#include <QString>
#include <QWidget>
#include "ui_Card.h"

class QMouseEvent;

QT_BEGIN_NAMESPACE
namespace Ui { class Card; }
QT_END_NAMESPACE

class Card : public QWidget
{
    Q_OBJECT

public:
    explicit Card(QWidget* parent = nullptr);
    ~Card() override;

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
    Ui::Card* ui;
    QString m_id;
};

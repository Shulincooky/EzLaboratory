#pragma once

#include "ExperimentParser.h"
#include "HomePageStateMachine.h"

#include <QList>
#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class HomePage; }
QT_END_NAMESPACE

class ExperimentCard;
class HomeNetworkDisconnected;
class HomeNoExperiment;
class QEvent;
class QObject;

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget* parent = nullptr);
    ~HomePage() override;

    void addExperimentCard(const ExperimentCardInfo& cardInfo, bool local);
    void clearExperimentCards();
    void applyExperimentFilter(const QString& filterText);
    void setNetworkDisconnected();
    void relayoutExperimentCards();

signals:
    void experimentSelected(const QString& id);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    int experimentCardColumnCount() const;
    bool hasVisibleExperimentCards() const;
    void refreshState();
    void setState(HomePageState state);

    Ui::HomePage* ui;
    HomeNoExperiment* m_noExperimentState = nullptr;
    HomeNetworkDisconnected* m_networkDisconnectedState = nullptr;
    QList<ExperimentCard*> m_experimentCards;
    HomePageState m_state = HomePageState::NoExperiment;
};

#pragma once

class HomeEmptyState;
class HomeNetworkDisconnected;
class HomeNoExperiment;
class QWidget;

enum class HomePageState
{
    Experiments,
    NoExperiment,
    NoSearchResult,
    NetworkDisconnected,
};

class HomePageStateMachine
{
public:
    HomePageStateMachine(
        QWidget* experimentPage,
        QWidget* statePageHost,
        HomeNoExperiment* noExperimentState,
        HomeNetworkDisconnected* networkDisconnectedState);

    HomePageState state() const;

    void refresh(bool hasExperimentCards, bool hasVisibleExperimentCards);
    void setState(HomePageState state);

private:
    HomePageState resolveState(
        bool hasExperimentCards,
        bool hasVisibleExperimentCards) const;
    HomeEmptyState* statePageFor(HomePageState state) const;
    void updateStatePageContent(HomePageState state);
    void showStatePage(HomeEmptyState* statePage);

    QWidget* m_experimentPage = nullptr;
    QWidget* m_statePageHost = nullptr;
    HomeNoExperiment* m_noExperimentState = nullptr;
    HomeNetworkDisconnected* m_networkDisconnectedState = nullptr;
    HomePageState m_state = HomePageState::NoExperiment;
};

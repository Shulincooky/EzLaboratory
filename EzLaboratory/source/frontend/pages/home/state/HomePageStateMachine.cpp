#include "HomePageStateMachine.h"

#include "HomeEmptyState.h"
#include "HomeNetworkDisconnected.h"
#include "HomeNoExperiment.h"

#include <QWidget>

HomePageStateMachine::HomePageStateMachine(
    QWidget* experimentPage,
    QWidget* statePageHost,
    HomeNoExperiment* noExperimentState,
    HomeNetworkDisconnected* networkDisconnectedState)
    : m_experimentPage(experimentPage)
    , m_statePageHost(statePageHost)
    , m_noExperimentState(noExperimentState)
    , m_networkDisconnectedState(networkDisconnectedState)
{
    setState(HomePageState::NoExperiment);
}

HomePageState HomePageStateMachine::state() const
{
    return m_state;
}

void HomePageStateMachine::refresh(bool hasExperimentCards, bool hasVisibleExperimentCards)
{
    setState(resolveState(hasExperimentCards, hasVisibleExperimentCards));
}

void HomePageStateMachine::setState(HomePageState state)
{
    m_state = state;
    updateStatePageContent(state);

    HomeEmptyState* statePage = statePageFor(state);
    m_experimentPage->setVisible(state == HomePageState::Experiments);
    m_statePageHost->setVisible(statePage != nullptr);
    showStatePage(statePage);
}

HomePageState HomePageStateMachine::resolveState(
    bool hasExperimentCards,
    bool hasVisibleExperimentCards) const
{
    if (m_state == HomePageState::NetworkDisconnected && !hasExperimentCards) {
        return HomePageState::NetworkDisconnected;
    }

    if (!hasExperimentCards) {
        return HomePageState::NoExperiment;
    }

    if (hasVisibleExperimentCards) {
        return HomePageState::Experiments;
    }

    return HomePageState::NoSearchResult;
}

HomeEmptyState* HomePageStateMachine::statePageFor(HomePageState state) const
{
    switch (state) {
    case HomePageState::Experiments:
        return nullptr;
    case HomePageState::NoExperiment:
    case HomePageState::NoSearchResult:
        return m_noExperimentState;
    case HomePageState::NetworkDisconnected:
        return m_networkDisconnectedState;
    }

    return nullptr;
}

void HomePageStateMachine::updateStatePageContent(HomePageState state)
{
    switch (state) {
    case HomePageState::Experiments:
    case HomePageState::NetworkDisconnected:
        break;
    case HomePageState::NoExperiment:
        m_noExperimentState->showNoExperimentLoaded();
        break;
    case HomePageState::NoSearchResult:
        m_noExperimentState->showNoSearchResult();
        break;
    }
}

void HomePageStateMachine::showStatePage(HomeEmptyState* statePage)
{
    m_noExperimentState->setVisible(statePage == m_noExperimentState);
    m_networkDisconnectedState->setVisible(statePage == m_networkDisconnectedState);
}

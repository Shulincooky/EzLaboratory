#include "HomePage.h"

#include "ExperimentCard.h"
#include "HomeNetworkDisconnected.h"
#include "HomeNoExperiment.h"
#include "ui_HomePage.h"

#include <QEvent>
#include <QMargins>

#include <algorithm>
#include <utility>

namespace
{
    constexpr int kExperimentCardFallbackWidth = 242;
}

HomePage::HomePage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HomePage())
{
    ui->setupUi(this);
    ui->experimentCardGridLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->homeScrollArea->viewport()->installEventFilter(this);

    m_noExperimentState = new HomeNoExperiment(ui->homeStateHostWidget);
    m_networkDisconnectedState = new HomeNetworkDisconnected(ui->homeStateHostWidget);

    ui->homeStateLayout->addWidget(m_noExperimentState);
    ui->homeStateLayout->addWidget(m_networkDisconnectedState);
    setState(HomePageState::NoExperiment);
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::addExperimentCard(const ExperimentCardInfo& cardInfo, bool local)
{
    auto* experimentCard = new ExperimentCard(ui->experimentCardsPage);
    experimentCard->setId(cardInfo.id);
    experimentCard->setTitle(cardInfo.title);
    experimentCard->setSummary(cardInfo.summary);
    experimentCard->setLocal(local);
    experimentCard->setProperty("searchText", cardInfo.searchText());

    connect(experimentCard, &ExperimentCard::clicked, this, &HomePage::experimentSelected);

    m_experimentCards.push_back(experimentCard);
    relayoutExperimentCards();
    refreshState();
}

void HomePage::clearExperimentCards()
{
    for (ExperimentCard* experimentCard : std::as_const(m_experimentCards)) {
        ui->experimentCardGridLayout->removeWidget(experimentCard);
        experimentCard->deleteLater();
    }

    m_experimentCards.clear();
    refreshState();
}

void HomePage::applyExperimentFilter(const QString& filterText)
{
    const QString needle = filterText.trimmed();

    for (ExperimentCard* experimentCard : std::as_const(m_experimentCards)) {
        const QString searchableText = experimentCard->property("searchText").toString();
        const bool matches = needle.isEmpty()
            || searchableText.contains(needle, Qt::CaseInsensitive);
        experimentCard->setVisible(matches);
    }

    relayoutExperimentCards();
    refreshState();
}

void HomePage::setNetworkDisconnected()
{
    setState(HomePageState::NetworkDisconnected);
}

void HomePage::relayoutExperimentCards()
{
    const int columnCount = experimentCardColumnCount();

    for (ExperimentCard* experimentCard : std::as_const(m_experimentCards)) {
        ui->experimentCardGridLayout->removeWidget(experimentCard);
    }

    int visibleIndex = 0;
    for (ExperimentCard* experimentCard : std::as_const(m_experimentCards)) {
        if (experimentCard->isHidden()) {
            continue;
        }

        const int row = visibleIndex / columnCount;
        const int column = visibleIndex % columnCount;
        ui->experimentCardGridLayout->addWidget(experimentCard, row, column, Qt::AlignLeft | Qt::AlignTop);
        ++visibleIndex;
    }
}

bool HomePage::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->homeScrollArea->viewport() && event->type() == QEvent::Resize) {
        relayoutExperimentCards();
    }

    return QWidget::eventFilter(watched, event);
}

int HomePage::experimentCardColumnCount() const
{
    const int cardWidth = m_experimentCards.isEmpty()
        ? kExperimentCardFallbackWidth
        : std::max(m_experimentCards.constFirst()->sizeHint().width(), kExperimentCardFallbackWidth);
    int horizontalSpacing = ui->experimentCardGridLayout->horizontalSpacing();
    if (horizontalSpacing < 0) {
        horizontalSpacing = std::max(ui->experimentCardGridLayout->spacing(), 0);
    }

    const QMargins outerMargins = ui->homeContentLayout->contentsMargins();
    const int availableWidth = std::max(
        1,
        ui->homeScrollArea->viewport()->width() - outerMargins.left() - outerMargins.right());

    return std::max(1, (availableWidth + horizontalSpacing) / (cardWidth + horizontalSpacing));
}

bool HomePage::hasVisibleExperimentCards() const
{
    return std::any_of(
        m_experimentCards.cbegin(),
        m_experimentCards.cend(),
        [](const ExperimentCard* experimentCard) {
            return !experimentCard->isHidden();
        });
}

void HomePage::refreshState()
{
    if (m_state == HomePageState::NetworkDisconnected && m_experimentCards.isEmpty()) {
        setState(HomePageState::NetworkDisconnected);
        return;
    }

    if (m_experimentCards.isEmpty()) {
        setState(HomePageState::NoExperiment);
        return;
    }

    setState(hasVisibleExperimentCards()
        ? HomePageState::Experiments
        : HomePageState::NoSearchResult);
}

void HomePage::setState(HomePageState state)
{
    m_state = state;

    switch (state) {
    case HomePageState::Experiments:
        ui->homeContentStack->setCurrentWidget(ui->homeScrollArea);
        break;
    case HomePageState::NoExperiment:
        m_noExperimentState->showNoExperimentLoaded();
        ui->homeContentStack->setCurrentWidget(ui->homeStatePage);
        break;
    case HomePageState::NoSearchResult:
        m_noExperimentState->showNoSearchResult();
        ui->homeContentStack->setCurrentWidget(ui->homeStatePage);
        break;
    case HomePageState::NetworkDisconnected:
        ui->homeContentStack->setCurrentWidget(ui->homeStatePage);
        break;
    }

    m_noExperimentState->setVisible(
        state == HomePageState::NoExperiment || state == HomePageState::NoSearchResult);
    m_networkDisconnectedState->setVisible(state == HomePageState::NetworkDisconnected);
}

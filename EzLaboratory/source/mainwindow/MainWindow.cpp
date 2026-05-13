#include "MainWindow.h"

#include "EzLaboratory.h"
#include "ExperimentCard.h"
#include "ExperimentReader.h"
#include "HistoryPage.h"
#include "HomeNetworkDisconnected.h"
#include "HomeNoExperiment.h"
#include "HomePageStateMachine.h"
#include "LoginDialog.h"
#include "ProfilePage.h"
#include "SettingsPage.h"
#include "ui_MainWindow.h"

#include <QAbstractButton>
#include <QDebug>
#include <QDialog>
#include <QEvent>
#include <QLineEdit>
#include <QMargins>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStyle>
#include <QStackedWidget>
#include <QTimer>
#include <QToolButton>

#include <algorithm>
#include <utility>

namespace
{
    constexpr int kExperimentCardFallbackWidth = 242;
    constexpr const char* kBuiltInExperimentConfigPath = ":/LocalExperiments/config";
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindowClass())
    , m_reader(new ExperimentReader(this))
{
    setWindowFlag(Qt::FramelessWindowHint);
    ui->setupUi(this);
    ui->gridLayout->setColumnStretch(0, 1);
    ui->gridLayout->setRowStretch(1, 1);
    ui->mainContentLayout->setColumnStretch(0, 1);
    ui->mainContentLayout->setRowStretch(0, 1);
    ui->workspacePageLayout->setColumnStretch(1, 1);
    ui->workspacePageLayout->setRowStretch(0, 1);
    ui->experimentCardGridLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->experimentScrollArea->viewport()->installEventFilter(this);
    setupHomeStates();
    setupUserPages();
    setupSettingsPage();
    setupLaboratoryPage();
    setupLaboratoryBackButton();
    setMainContentPage(MainContentPage::Experiments);

    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::applyExperimentFilter);
    connect(ui->navExperimentsButton, &QToolButton::clicked, this, [this]() {
        setMainContentPage(MainContentPage::Experiments);
        resetExperimentFilter();
    });
    connect(m_reader, &ExperimentReader::remotePackFailed, this, [this](const QUrl&, const QString&) {
        clearExperimentCards();
        m_experimentCache.clear();
        m_homePageStateMachine->setState(HomePageState::NetworkDisconnected);
    });
    connect(ui->avatarButton, &QToolButton::clicked, this, [this]() {
        openUserPage(MainContentPage::Profile);
    });
    connect(ui->avatarButton, &QToolButton::toggled, this, [this](bool checked) {
        ui->avatarWidget->setProperty("selected", checked);
        ui->avatarWidget->style()->unpolish(ui->avatarWidget);
        ui->avatarWidget->style()->polish(ui->avatarWidget);
        ui->avatarWidget->update();
    });
    connect(ui->historyButton, &QToolButton::clicked, this, [this]() {
        openUserPage(MainContentPage::History);
    });
    connect(ui->themeButton, &QToolButton::clicked, this, [this]() {
        qInfo() << "[MainWindow] theme toggle clicked.";
    });
    connect(ui->settingsButton, &QToolButton::clicked, this, [this]() {
        setMainContentPage(MainContentPage::Settings);
    });

    connect(ui->minimizeButton, &QToolButton::clicked, this, &MainWindow::showMinimized);
    connect(ui->maximizeButton, &QToolButton::clicked, this, &MainWindow::toggleWindowMaximizeState);
    connect(ui->closeButton, &QToolButton::clicked, this, [this]() {
        ui->closeButton->setEnabled(false);
        QTimer::singleShot(0, this, &QWidget::close);
    });

    loadBuiltInExperiments();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_draggingWindow = false;

    if (ui && ui->closeButton) {
        ui->closeButton->setEnabled(false);
    }

    QMainWindow::closeEvent(event);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->experimentScrollArea->viewport() && event->type() == QEvent::Resize) {
        relayoutExperimentCards();
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::toggleWindowMaximizeState()
{
    if (isMaximized()) {
        showNormal();
        return;
    }

    showMaximized();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isWindowTitleBarDragArea(event->position().toPoint())) {
        toggleWindowMaximizeState();
        event->accept();
        return;
    }

    QMainWindow::mouseDoubleClickEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isWindowTitleBarDragArea(event->position().toPoint())) {
        m_draggingWindow = true;
        m_dragOffset = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_draggingWindow = false;
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_draggingWindow && !(event->buttons() & Qt::LeftButton)) {
        m_draggingWindow = false;
    }

    if (m_draggingWindow && !isMaximized()) {
        move(event->globalPosition().toPoint() - m_dragOffset);
        event->accept();
        return;
    }

    QMainWindow::mouseMoveEvent(event);
}

bool MainWindow::isWindowTitleBarDragArea(const QPoint& position) const
{
    if (!ui->topBar->geometry().contains(position)) {
        return false;
    }

    QWidget* hoveredWidget = childAt(position);
    while (hoveredWidget != nullptr && hoveredWidget != ui->topBar) {
        if (hoveredWidget == ui->searchFrame
            || hoveredWidget == ui->searchIconLabel
            || qobject_cast<QAbstractButton*>(hoveredWidget) != nullptr
            || qobject_cast<QLineEdit*>(hoveredWidget) != nullptr) {
            return false;
        }
        hoveredWidget = hoveredWidget->parentWidget();
    }

    return true;
}

void MainWindow::loadBuiltInExperiments()
{
    clearExperimentCards();
    m_experimentCache.clear();

    QString errorString;
    const QList<ExperimentReadResult> experiments = m_reader->readLocalDirectory(
        QString::fromLatin1(kBuiltInExperimentConfigPath),
        &errorString);

    if (experiments.isEmpty()) {
        qWarning() << "[MainWindow] failed to load built-in experiments:" << errorString;
        updateHomePageState();
        return;
    }

    for (const ExperimentReadResult& experiment : experiments) {
        if (!cacheExperiment(experiment.data, experiment.local)) {
            qWarning() << "[MainWindow] skipped experiment config:" << experiment.origin;
        }
    }

    applyExperimentFilter(ui->searchLineEdit->text());
}

bool MainWindow::cacheExperiment(const QByteArray& data, bool local)
{
    const ExperimentParseResult result = m_experimentParser.parseExperiment(data);
    if (!result.ok) {
        qWarning() << "[MainWindow] failed to parse experiment:" << result.errorString;
        return false;
    }

    const QString id = result.experiment.card.id;
    if (m_experimentCache.contains(id)) {
        qWarning() << "[MainWindow] duplicate experiment id skipped:" << id;
        return false;
    }

    m_experimentCache.insert(id, result.experiment);
    addExperimentCard(m_experimentParser.cardInfo(result.experiment), local);
    return true;
}

void MainWindow::addExperimentCard(const ExperimentCardInfo& cardInfo, bool local)
{
    auto* experimentCard = new ExperimentCard(ui->experimentCardGridWidget);
    experimentCard->setId(cardInfo.id);
    experimentCard->setTitle(cardInfo.title);
    experimentCard->setSummary(cardInfo.summary);
    experimentCard->setLocal(local);
    experimentCard->setProperty("searchText", cardInfo.searchText());

    connect(experimentCard, &ExperimentCard::clicked, this, [this](const QString& id) {
        openExperiment(id);
        });

    m_experimentCards.push_back(experimentCard);
    relayoutExperimentCards();
}

void MainWindow::clearExperimentCards()
{
    for (ExperimentCard* experimentCard : std::as_const(m_experimentCards)) {
        ui->experimentCardGridLayout->removeWidget(experimentCard);
        experimentCard->deleteLater();
    }
    m_experimentCards.clear();
}

void MainWindow::relayoutExperimentCards()
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

int MainWindow::experimentCardColumnCount() const
{
    const int cardWidth = m_experimentCards.isEmpty()
        ? kExperimentCardFallbackWidth
        : std::max(m_experimentCards.constFirst()->sizeHint().width(), kExperimentCardFallbackWidth);
    int horizontalSpacing = ui->experimentCardGridLayout->horizontalSpacing();
    if (horizontalSpacing < 0) {
        horizontalSpacing = std::max(ui->experimentCardGridLayout->spacing(), 0);
    }

    const QMargins outerMargins = ui->gridLayout_2->contentsMargins();
    const int availableWidth = std::max(
        1,
        ui->experimentScrollArea->viewport()->width() - outerMargins.left() - outerMargins.right());

    return std::max(1, (availableWidth + horizontalSpacing) / (cardWidth + horizontalSpacing));
}

void MainWindow::applyExperimentFilter(const QString& filterText)
{
    const QString needle = filterText.trimmed();

    for (ExperimentCard* experimentCard : std::as_const(m_experimentCards)) {
        const QString searchableText = experimentCard->property("searchText").toString();
        const bool matches = needle.isEmpty()
            || searchableText.contains(needle, Qt::CaseInsensitive);
        experimentCard->setVisible(matches);
    }

    relayoutExperimentCards();
    updateHomePageState();
}

void MainWindow::resetExperimentFilter()
{
    if (ui->searchLineEdit->text().isEmpty()) {
        applyExperimentFilter(QString());
        return;
    }

    ui->searchLineEdit->clear();
}

void MainWindow::openExperiment(const QString& id)
{
    const auto experiment = m_experimentCache.constFind(id);
    if (experiment == m_experimentCache.cend()) {
        QMessageBox::warning(
            this,
            QStringLiteral("Experiment unavailable"),
            QStringLiteral("The selected experiment is not in the current cache."));
        return;
    }

    m_laboratoryPage->initializeExperiment(experiment.value());
    m_pageBeforeLaboratory = m_mainContentPage == MainContentPage::Laboratory
        ? MainContentPage::Experiments
        : m_mainContentPage;
    setMainContentPage(MainContentPage::Laboratory);
}

void MainWindow::returnFromLaboratory()
{
    setMainContentPage(m_pageBeforeLaboratory);
}

void MainWindow::setupHomeStates()
{
    m_noExperimentState = new HomeNoExperiment(ui->homeStateHostWidget);
    m_networkDisconnectedState = new HomeNetworkDisconnected(ui->homeStateHostWidget);

    ui->homeStateLayout->addWidget(m_noExperimentState);
    ui->homeStateLayout->addWidget(m_networkDisconnectedState);
    m_homePageStateMachine = std::make_unique<HomePageStateMachine>(
        ui->experimentCardGridWidget,
        ui->homeStateHostWidget,
        m_noExperimentState,
        m_networkDisconnectedState);
}

void MainWindow::setupUserPages()
{
    m_profilePage = new ProfilePage(ui->workspaceContentStack);
    m_historyPage = new HistoryPage(ui->workspaceContentStack);

    ui->workspaceContentStack->addWidget(m_profilePage);
    ui->workspaceContentStack->addWidget(m_historyPage);
}

void MainWindow::setupSettingsPage()
{
    m_settingsPage = new SettingsPage(ui->workspaceContentStack);
    ui->workspaceContentStack->addWidget(m_settingsPage);
}

void MainWindow::setupLaboratoryPage()
{
    m_laboratoryPage = new EzLaboratory(ui->mainContentStack);
    ui->mainContentStack->addWidget(m_laboratoryPage);
}

void MainWindow::setupLaboratoryBackButton()
{
    m_backButton = new QToolButton(ui->topBar);
    m_backButton->setText(QStringLiteral("< 返回"));
    m_backButton->setToolTip(QStringLiteral("返回实验列表"));
    m_backButton->setCursor(Qt::PointingHandCursor);
    m_backButton->setFixedSize(76, 36);
    m_backButton->setStyleSheet(QStringLiteral(
        "QToolButton {"
        "background: #F3F4F6;"
        "border: 1px solid #D6D6D6;"
        "border-radius: 8px;"
        "color: #3D3D3D;"
        "font-size: 13px;"
        "font-weight: 600;"
        "}"
        "QToolButton:hover {"
        "background: #E6E6E6;"
        "}"
        "QToolButton:pressed {"
        "background: #DADADA;"
        "}"));
    positionLaboratoryBackButton();
    m_backButton->hide();

    connect(m_backButton, &QToolButton::clicked, this, &MainWindow::returnFromLaboratory);
}

void MainWindow::positionLaboratoryBackButton()
{
    if (!m_backButton) {
        return;
    }

    m_backButton->move(176, 17);
    m_backButton->raise();
}

void MainWindow::updateHomePageState()
{
    const bool hasVisibleExperimentCard = std::any_of(
        m_experimentCards.cbegin(),
        m_experimentCards.cend(),
        [](const ExperimentCard* experimentCard) {
            return !experimentCard->isHidden();
        });

    m_homePageStateMachine->refresh(
        !m_experimentCards.isEmpty(),
        hasVisibleExperimentCard);
}

void MainWindow::openUserPage(MainContentPage page)
{
    const MainContentPage previousPage = m_mainContentPage;
    if (!ensureUserLoggedIn()) {
        setMainContentPage(previousPage);
        return;
    }

    setMainContentPage(page);
}

bool MainWindow::ensureUserLoggedIn()
{
    if (m_userLoggedIn) {
        return true;
    }

    LoginDialog loginDialog(this);
    if (loginDialog.exec() != QDialog::Accepted) {
        return false;
    }

    m_userLoggedIn = true;
    m_userEmail = loginDialog.email();
    m_userNickname = loginDialog.nickname();

    if (m_profilePage) {
        m_profilePage->setUserIdentity(m_userEmail, m_userNickname);
    }

    return true;
}

void MainWindow::setMainContentPage(MainContentPage page)
{
    m_mainContentPage = page;

    const bool laboratoryVisible = page == MainContentPage::Laboratory;
    if (m_backButton) {
        positionLaboratoryBackButton();
        m_backButton->setVisible(laboratoryVisible);
    }
    ui->searchFrame->setVisible(!laboratoryVisible);

    if (laboratoryVisible) {
        ui->mainContentStack->setCurrentWidget(m_laboratoryPage);
    } else {
        ui->mainContentStack->setCurrentWidget(ui->workspacePage);
        if (page == MainContentPage::Experiments) {
            ui->workspaceContentStack->setCurrentWidget(ui->experimentScrollArea);
            relayoutExperimentCards();
        } else if (page == MainContentPage::Profile && m_profilePage) {
            ui->workspaceContentStack->setCurrentWidget(m_profilePage);
        } else if (page == MainContentPage::History && m_historyPage) {
            ui->workspaceContentStack->setCurrentWidget(m_historyPage);
        } else if (page == MainContentPage::Settings && m_settingsPage) {
            ui->workspaceContentStack->setCurrentWidget(m_settingsPage);
        }
    }

    if (page == MainContentPage::Experiments) {
        ui->navExperimentsButton->setChecked(true);
    } else if (page == MainContentPage::Profile) {
        ui->avatarButton->setChecked(true);
    } else if (page == MainContentPage::History) {
        ui->historyButton->setChecked(true);
    } else if (page == MainContentPage::Settings) {
        ui->settingsButton->setChecked(true);
    }
}

void MainWindow::showPendingFeatureMessage(const QString& title, const QString& message)
{
    QMessageBox::information(this, title, message);
}

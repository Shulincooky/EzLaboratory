#include "MainWindow.h"

#include "EzLaboratory.h"
#include "ExperimentReader.h"
#include "HistoryPage.h"
#include "HomePage.h"
#include "LoginDialog.h"
#include "ProfilePage.h"
#include "SettingsPage.h"
#include "TopBarBackButton.h"
#include "ui_MainWindow.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QIODevice>
#include <QLineEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStyle>
#include <QStackedWidget>
#include <QTimer>
#include <QToolButton>

namespace
{
    constexpr const char* kBuiltInExperimentConfigPath = ":/LocalExperiments/config";
    constexpr const char* kMainWindowBaseStylePath = ":/Main/styles/mainwindow/base.qss";
    constexpr const char* kMainWindowLightStylePath = ":/Main/styles/mainwindow/light.qss";

    QString readStyleSheetResource(const char* resourcePath)
    {
        QFile file(QString::fromLatin1(resourcePath));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "[MainWindow] failed to load stylesheet:" << resourcePath << file.errorString();
            return {};
        }

        return QString::fromUtf8(file.readAll());
    }
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindowClass())
    , m_reader(new ExperimentReader(this))
{
    setWindowFlag(Qt::FramelessWindowHint);
    ui->setupUi(this);
    applyMainWindowStyle();
    ui->gridLayout->setColumnStretch(0, 1);
    ui->gridLayout->setRowStretch(1, 1);
    ui->workspaceLayout->setColumnStretch(1, 1);
    ui->workspaceLayout->setRowStretch(0, 1);
    setupSidebarNavigation();
    setupHomePage();
    setupUserPages();
    setupSettingsPage();
    setupLaboratoryPage();
    setupLaboratoryBackButton();
    setMainContentPage(MainContentPage::Experiments);

    connect(ui->searchLineEdit, &QLineEdit::textChanged, m_homePage, &HomePage::applyExperimentFilter);
    connect(ui->navExperimentsButton, &QToolButton::clicked, this, [this]() {
        setMainContentPage(MainContentPage::Experiments);
        resetExperimentFilter();
    });
    connect(m_reader, &ExperimentReader::remotePackFailed, this, [this](const QUrl&, const QString&) {
        m_homePage->clearExperimentCards();
        m_experimentCache.clear();
        m_homePage->setNetworkDisconnected();
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

void MainWindow::applyMainWindowStyle()
{
    const QString baseStyle = readStyleSheetResource(kMainWindowBaseStylePath);
    const QString paletteStyle = readStyleSheetResource(kMainWindowLightStylePath);

    setStyleSheet(baseStyle + QLatin1Char('\n') + paletteStyle);
}

void MainWindow::setupSidebarNavigation()
{
    auto* sideBarButtonGroup = new QButtonGroup(this);
    sideBarButtonGroup->setObjectName(QStringLiteral("sideBarButtonGroup"));
    sideBarButtonGroup->setExclusive(true);
    sideBarButtonGroup->addButton(ui->navExperimentsButton, static_cast<int>(MainContentPage::Experiments));
    sideBarButtonGroup->addButton(ui->avatarButton, static_cast<int>(MainContentPage::Profile));
    sideBarButtonGroup->addButton(ui->historyButton, static_cast<int>(MainContentPage::History));
    sideBarButtonGroup->addButton(ui->settingsButton, static_cast<int>(MainContentPage::Settings));
}

void MainWindow::loadBuiltInExperiments()
{
    m_homePage->clearExperimentCards();
    m_experimentCache.clear();

    QString errorString;
    const QList<ExperimentReadResult> experiments = m_reader->readLocalDirectory(
        QString::fromLatin1(kBuiltInExperimentConfigPath),
        &errorString);

    if (experiments.isEmpty()) {
        qWarning() << "[MainWindow] failed to load built-in experiments:" << errorString;
        return;
    }

    for (const ExperimentReadResult& experiment : experiments) {
        if (!cacheExperiment(experiment.data, experiment.local)) {
            qWarning() << "[MainWindow] skipped experiment config:" << experiment.origin;
        }
    }

    m_homePage->applyExperimentFilter(ui->searchLineEdit->text());
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
    m_homePage->addExperimentCard(m_experimentParser.cardInfo(result.experiment), local);
    return true;
}

void MainWindow::resetExperimentFilter()
{
    if (ui->searchLineEdit->text().isEmpty()) {
        m_homePage->applyExperimentFilter(QString());
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

void MainWindow::setupHomePage()
{
    m_homePage = new HomePage(ui->workspaceContentStack);
    ui->workspaceContentStack->addWidget(m_homePage);

    connect(m_homePage, &HomePage::experimentSelected, this, &MainWindow::openExperiment);
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
    m_backButton = new TopBarBackButton(ui->topBar);
    positionLaboratoryBackButton();
    m_backButton->hide();

    connect(m_backButton, &TopBarBackButton::clicked, this, &MainWindow::returnFromLaboratory);
}

void MainWindow::positionLaboratoryBackButton()
{
    if (!m_backButton) {
        return;
    }

    m_backButton->move(176, 17);
    m_backButton->raise();
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
        ui->mainContentStack->setCurrentWidget(ui->workspace);
        if (page == MainContentPage::Experiments) {
            ui->workspaceContentStack->setCurrentWidget(m_homePage);
            m_homePage->relayoutExperimentCards();
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

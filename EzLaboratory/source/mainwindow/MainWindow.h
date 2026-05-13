#pragma once

#include "ExperimentParser.h"

#include <QHash>
#include <QMainWindow>
#include <QList>
#include <QPoint>
#include <QString>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class ExperimentCard;
class ExperimentReader;
class EzLaboratory;
class HistoryPage;
class HomeNetworkDisconnected;
class HomePageStateMachine;
class HomeNoExperiment;
class ProfilePage;
class QCloseEvent;
class QEvent;
class QMouseEvent;
class QObject;
class QToolButton;
class SettingsPage;
class QWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void toggleWindowMaximizeState();

private:
    enum class MainContentPage
    {
        Experiments,
        Profile,
        History,
        Laboratory,
        Settings,
    };

    // Frameless window controls.
    bool isWindowTitleBarDragArea(const QPoint& position) const;

    // Experiment cards.
    void loadBuiltInExperiments();
    bool cacheExperiment(const QByteArray& data, bool local);
    void addExperimentCard(const ExperimentCardInfo& cardInfo, bool local);
    void clearExperimentCards();
    void relayoutExperimentCards();
    int experimentCardColumnCount() const;
    void applyExperimentFilter(const QString& filterText);
    void resetExperimentFilter();
    void openExperiment(const QString& id);
    void returnFromLaboratory();

    // Home state pages and general UI helpers.
    void setupHomeStates();
    void setupUserPages();
    void setupSettingsPage();
    void setupLaboratoryPage();
    void setupLaboratoryBackButton();
    void positionLaboratoryBackButton();
    void updateHomePageState();
    void openUserPage(MainContentPage page);
    bool ensureUserLoggedIn();
    void setMainContentPage(MainContentPage page);
    void showPendingFeatureMessage(const QString& title, const QString& message);

    Ui::MainWindowClass *ui;
    ExperimentReader* m_reader = nullptr;
    ExperimentParser m_experimentParser;
    HomeNoExperiment* m_noExperimentState = nullptr;
    HomeNetworkDisconnected* m_networkDisconnectedState = nullptr;
    std::unique_ptr<HomePageStateMachine> m_homePageStateMachine;
    ProfilePage* m_profilePage = nullptr;
    HistoryPage* m_historyPage = nullptr;
    SettingsPage* m_settingsPage = nullptr;
    EzLaboratory* m_laboratoryPage = nullptr;
    QToolButton* m_backButton = nullptr;
    QList<ExperimentCard*> m_experimentCards;
    QHash<QString, ExperimentDefinition> m_experimentCache;
    MainContentPage m_pageBeforeLaboratory = MainContentPage::Experiments;
    MainContentPage m_mainContentPage = MainContentPage::Experiments;
    QString m_userEmail;
    QString m_userNickname;
    QPoint m_dragOffset;
    bool m_userLoggedIn = false;
    bool m_draggingWindow = false;
};


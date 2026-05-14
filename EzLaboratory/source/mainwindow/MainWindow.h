#pragma once

#include "ExperimentParser.h"

#include <QHash>
#include <QList>
#include <QMainWindow>
#include <QPoint>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class ExperimentReader;
class EzLaboratory;
class QEvent;
class HistoryPage;
class HomePage;
class ProfilePage;
class QCloseEvent;
class QMouseEvent;
class QObject;
class QResizeEvent;
class SettingsPage;
class TopBarBackButton;
class QWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

protected:
    void changeEvent(QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

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
    QWidget* createResizeHandle(Qt::Edges edges, Qt::CursorShape cursorShape);
    bool isWindowTitleBarDragArea(const QPoint& position) const;
    void setupResizeHandles();
    void updateResizeHandles();
    void updateMaximizeButtonIcon();

    // Experiment cards.
    void loadBuiltInExperiments();
    bool cacheExperiment(const QByteArray& data, bool local);
    void resetExperimentFilter();
    void openExperiment(const QString& id);
    void returnFromLaboratory();

    // Page setup and general UI helpers.
    void applyMainWindowStyle();
    void setupSidebarNavigation();
    void setupHomePage();
    void setupUserPages();
    void setupSettingsPage();
    void setupLaboratoryPage();
    void setupLaboratoryBackButton();
    void positionLaboratoryBackButton();
    void openUserPage(MainContentPage page);
    void setMainContentPage(MainContentPage page);
    void showPendingFeatureMessage(const QString& title, const QString& message);

    Ui::MainWindowClass *ui;
    ExperimentReader* m_reader = nullptr;
    ExperimentParser m_experimentParser;
    HomePage* m_homePage = nullptr;
    ProfilePage* m_profilePage = nullptr;
    HistoryPage* m_historyPage = nullptr;
    SettingsPage* m_settingsPage = nullptr;
    EzLaboratory* m_laboratoryPage = nullptr;
    TopBarBackButton* m_backButton = nullptr;
    QList<QWidget*> m_resizeHandles;
    QHash<QString, ExperimentDefinition> m_experimentCache;
    MainContentPage m_pageBeforeLaboratory = MainContentPage::Experiments;
    MainContentPage m_mainContentPage = MainContentPage::Experiments;
    QPoint m_dragOffset;
    bool m_draggingWindow = false;
};


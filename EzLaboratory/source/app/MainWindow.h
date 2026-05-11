#pragma once

#include <QMainWindow>
#include <QList>
#include <QPoint>
#include <QString>
#include "ui_MainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class Card;
class ExperimentReader;
class QCloseEvent;
class QMouseEvent;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    struct CardData
    {
        QString id;
        QString title;
        QString summary;
        QString tag;
        bool local = false;
    };

    void loadDefaultExperiments();
    void addExperimentCard(const CardData& data);
    void clearExperimentCards();
    void applyExperimentFilter(const QString& filterText);
    void resetExperimentFilter();
    void updateEmptyHint();
    void showPendingFeatureMessage(const QString& title, const QString& message);
    bool isTopBarDragArea(const QPoint& position) const;
    void requestClose();
    void toggleMaximized();

    CardData parseExperimentCard(const QByteArray& data,
        bool local,
        const QString& fallbackId) const;

	Ui::MainWindowClass *ui;
    ExperimentReader* m_reader = nullptr;
    QList<Card*> m_cards;
    QPoint m_dragOffset;
    bool m_draggingWindow = false;
};


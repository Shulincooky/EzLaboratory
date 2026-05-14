#pragma once

#include "ExperimentParser.h"

#include <QList>
#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class HomePage; }
QT_END_NAMESPACE

class ExperimentCard;
class HomeEmptyState;
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
    enum class State
    {
        Experiments,
        NoExperiment,
        NoSearchResult,
        NetworkDisconnected,
    };

    int experimentCardColumnCount() const;
    bool hasVisibleExperimentCards() const;
    void refreshState();
    void setState(State state);
    void showStateText(const QString& title, const QString& message);

    Ui::HomePage* ui;
    HomeEmptyState* m_stateView = nullptr;
    QList<ExperimentCard*> m_experimentCards;
    State m_state = State::NoExperiment;
};

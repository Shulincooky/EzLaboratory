#include "ExperimentCard.h"

#include "ui_ExperimentCard.h"

#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

ExperimentCard::ExperimentCard(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ExperimentCard())
{
    ui->setupUi(this);
    setCursor(Qt::PointingHandCursor);
    setLocal(false);

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(22.0);
    shadow->setOffset(0.0, 6.0);
    shadow->setColor(QColor(31, 41, 55, 38));
    ui->panelFrame->setGraphicsEffect(shadow);
}

ExperimentCard::~ExperimentCard()
{
    delete ui;
}

QString ExperimentCard::id() const
{
    return m_id;
}

void ExperimentCard::setId(const QString& id)
{
    m_id = id;
}

void ExperimentCard::setTitle(const QString& title)
{
    ui->titleLabel->setText(title);
}

void ExperimentCard::setSummary(const QString& summary)
{
    ui->summaryLabel->setText(summary);
}

void ExperimentCard::setLocal(bool local)
{
}

void ExperimentCard::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->position().toPoint())) {
        emit clicked(m_id);
    }

    QWidget::mouseReleaseEvent(event);
}

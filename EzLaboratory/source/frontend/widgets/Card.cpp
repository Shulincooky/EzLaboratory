#include "Card.h"

#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

Card::Card(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Card())
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

Card::~Card()
{
    delete ui;
}

QString Card::id() const
{
    return m_id;
}

void Card::setId(const QString& id)
{
    m_id = id;
}

void Card::setTitle(const QString& title)
{
    ui->titleLabel->setText(title);
}

void Card::setSummary(const QString& summary)
{
    ui->summaryLabel->setText(summary);
}

void Card::setLocal(bool local)
{
}

void Card::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->position().toPoint())) {
        emit clicked(m_id);
    }

    QWidget::mouseReleaseEvent(event);
}

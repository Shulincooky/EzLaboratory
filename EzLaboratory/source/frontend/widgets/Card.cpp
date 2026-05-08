#include "Card.h"

#include "ui_Card.h"

#include <QMouseEvent>

Card::Card(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Card())
{
    ui->setupUi(this);
    setCursor(Qt::PointingHandCursor);
    setLocal(false);
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

void Card::setTag(const QString& tag)
{
    ui->tagLabel->setText(tag);
    ui->tagLabel->setVisible(!tag.trimmed().isEmpty());
}

void Card::setLocal(bool local)
{
    ui->localBadgeLabel->setVisible(local);
}

void Card::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->position().toPoint())) {
        emit clicked(m_id);
    }

    QWidget::mouseReleaseEvent(event);
}

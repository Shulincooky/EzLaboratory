#include "ExperimentCard.h"

#include "ui_ExperimentCard.h"

#include <QFontMetrics>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QMouseEvent>
#include <QSize>
#include <QStringList>

#include <algorithm>

namespace
{
    constexpr QSize kExperimentCardSize(242, 231);
    constexpr int kCardTextWidth = 190;
    constexpr int kElideRightPadding = 8;
    constexpr int kSummaryLineCount = 2;

    QString elidedLabelText(const QString& text, QLabel* label)
    {
        label->ensurePolished();
        const int availableWidth = std::max(1, label->contentsRect().width() - kElideRightPadding);
        return label->fontMetrics().elidedText(text, Qt::ElideRight, availableWidth);
    }

    QString elidedMultilineText(const QString& text, QLabel* label, int lineCount)
    {
        label->ensurePolished();

        const QFontMetrics metrics(label->font());
        const int availableWidth = std::max(1, label->contentsRect().width() - kElideRightPadding);
        QString remaining = text.simplified();
        QStringList lines;

        for (int line = 0; line < lineCount && !remaining.isEmpty(); ++line) {
            if (line == lineCount - 1 || metrics.horizontalAdvance(remaining) <= availableWidth) {
                lines.push_back(metrics.elidedText(remaining, Qt::ElideRight, availableWidth));
                break;
            }

            int end = remaining.size();
            while (end > 0 && metrics.horizontalAdvance(remaining.left(end)) > availableWidth) {
                --end;
            }

            if (end <= 0) {
                lines.push_back(metrics.elidedText(remaining, Qt::ElideRight, availableWidth));
                break;
            }

            lines.push_back(remaining.left(end).trimmed());
            remaining = remaining.mid(end).trimmed();
        }

        return lines.join(QLatin1Char('\n'));
    }
}

ExperimentCard::ExperimentCard(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ExperimentCard())
{
    ui->setupUi(this);
    setFixedSize(kExperimentCardSize);
    setCursor(Qt::PointingHandCursor);
    setLocal(false);

    ui->titleLabel->setFixedWidth(kCardTextWidth);
    ui->titleLabel->setWordWrap(false);
    ui->summaryLabel->setFixedWidth(kCardTextWidth);
    ui->summaryLabel->setWordWrap(true);
    ui->summaryLabel->setFixedHeight(ui->summaryLabel->fontMetrics().lineSpacing() * kSummaryLineCount);

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
    ui->titleLabel->setText(elidedLabelText(title, ui->titleLabel));
    ui->titleLabel->setToolTip(title);
}

void ExperimentCard::setSummary(const QString& summary)
{
    ui->summaryLabel->setText(elidedMultilineText(summary, ui->summaryLabel, kSummaryLineCount));
    ui->summaryLabel->setToolTip(summary);
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

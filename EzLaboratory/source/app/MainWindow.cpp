#include "MainWindow.h"

#include "Card.h"
#include "ExperimentReader.h"

#include <QAbstractButton>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTimer>
#include <QToolButton>

#include <utility>

namespace
{
    constexpr int kCardColumns = 3;
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindowClass())
    , m_reader(new ExperimentReader(this))
{
    setWindowFlag(Qt::FramelessWindowHint);
    ui->setupUi(this);
    ui->gridLayout->setColumnStretch(1, 1);
    ui->gridLayout->setRowStretch(1, 1);
    ui->cardGridLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::applyExperimentFilter);
    connect(ui->navExperimentsButton, &QToolButton::clicked, this, &MainWindow::resetExperimentFilter);
    connect(ui->avatarButton, &QToolButton::clicked, this, [this]() {
        showPendingFeatureMessage(
            QStringLiteral("个人信息"),
            QStringLiteral("这里先作为头像占位，用户资料功能等你确认后再接入。"));
    });
    connect(ui->historyButton, &QToolButton::clicked, this, [this]() {
        showPendingFeatureMessage(
            QStringLiteral("历史记录"),
            QStringLiteral("历史记录入口已经连接，记录来源和展示规则等你确认后再实现。"));
    });
    connect(ui->themeButton, &QToolButton::clicked, this, [this]() {
        showPendingFeatureMessage(
            QStringLiteral("主题切换"),
            QStringLiteral("主题入口已经连接，深色主题样式等你确认后再实现。"));
    });
    connect(ui->settingsButton, &QToolButton::clicked, this, [this]() {
        showPendingFeatureMessage(
            QStringLiteral("设置"),
            QStringLiteral("设置入口已经连接，具体配置项等你确认后再实现。"));
    });
    connect(ui->minimizeButton, &QToolButton::clicked, this, &MainWindow::showMinimized);
    connect(ui->maximizeButton, &QToolButton::clicked, this, &MainWindow::toggleMaximized);
    connect(ui->closeButton, &QToolButton::clicked, this, &MainWindow::requestClose);

    loadDefaultExperiments();
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

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isTopBarDragArea(event->position().toPoint())) {
        toggleMaximized();
        event->accept();
        return;
    }

    QMainWindow::mouseDoubleClickEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_draggingWindow && !isMaximized()) {
        move(event->globalPosition().toPoint() - m_dragOffset);
        event->accept();
        return;
    }

    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isTopBarDragArea(event->position().toPoint())) {
        m_draggingWindow = true;
        m_dragOffset = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
        return;
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_draggingWindow) {
        m_draggingWindow = false;
        event->accept();
        return;
    }

    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::loadDefaultExperiments()
{
    clearExperimentCards();

    QString errorString;
    const QByteArray data = m_reader->readLocalFile(
        QStringLiteral(":/EzLaboratory/config/default_cuso4_naoh.json"),
        &errorString);

    if (data.isEmpty()) {
        qWarning() << "[MainWindow] failed to load default experiment:" << errorString;
        updateEmptyHint();
        return;
    }

    addExperimentCard(parseExperimentCard(
        data,
        true,
        QStringLiteral("local.default")));

    applyExperimentFilter(ui->searchLineEdit->text());
}

void MainWindow::addExperimentCard(const CardData& data)
{
    auto* card = new Card(ui->cardGridWidget);
    card->setId(data.id);
    card->setTitle(data.title);
    card->setSummary(data.summary);
    card->setLocal(data.local);
    card->setProperty(
        "searchText",
        QStringLiteral("%1\n%2\n%3\n%4").arg(data.id, data.title, data.summary, data.tag));

    connect(card, &Card::clicked, this, [this](const QString& id) {
        qDebug() << "[MainWindow] experiment card clicked:" << id;
        });

    const int index = m_cards.size();
    const int row = index / kCardColumns;
    const int column = index % kCardColumns;
    ui->cardGridLayout->addWidget(card, row, column, Qt::AlignLeft | Qt::AlignTop);
    m_cards.push_back(card);
}

void MainWindow::clearExperimentCards()
{
    for (Card* card : std::as_const(m_cards)) {
        ui->cardGridLayout->removeWidget(card);
        card->deleteLater();
    }
    m_cards.clear();
}

void MainWindow::applyExperimentFilter(const QString& filterText)
{
    const QString needle = filterText.trimmed();

    for (Card* card : std::as_const(m_cards)) {
        const QString searchableText = card->property("searchText").toString();
        const bool matches = needle.isEmpty()
            || searchableText.contains(needle, Qt::CaseInsensitive);
        card->setVisible(matches);
    }

    updateEmptyHint();
}

void MainWindow::resetExperimentFilter()
{
    if (ui->searchLineEdit->text().isEmpty()) {
        applyExperimentFilter(QString());
        return;
    }

    ui->searchLineEdit->clear();
}

void MainWindow::updateEmptyHint()
{
    bool hasVisibleCard = false;
    for (Card* card : std::as_const(m_cards)) {
        if (!card->isHidden()) {
            hasVisibleCard = true;
            break;
        }
    }

    ui->emptyHintFrame->setVisible(!hasVisibleCard);
    if (m_cards.isEmpty()) {
        ui->emptyHintTitleLabel->setText(QStringLiteral("暂无实验加载"));
        ui->emptyHintBodyLabel->setText(
            QStringLiteral("请检查网络连接或网络代理，实验将在正常连接服务器后加载"));
        return;
    }

    ui->emptyHintTitleLabel->setText(QStringLiteral("没有匹配的实验"));
    ui->emptyHintBodyLabel->setText(QStringLiteral("换个关键词试试，或点击全部实验清除搜索。"));
}

void MainWindow::showPendingFeatureMessage(const QString& title, const QString& message)
{
    QMessageBox::information(this, title, message);
}

bool MainWindow::isTopBarDragArea(const QPoint& position) const
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

void MainWindow::requestClose()
{
    ui->closeButton->setEnabled(false);
    QTimer::singleShot(0, this, [this]() {
        close();
    });
}

void MainWindow::toggleMaximized()
{
    if (isMaximized()) {
        showNormal();
        return;
    }

    showMaximized();
}

MainWindow::CardData MainWindow::parseExperimentCard(
    const QByteArray& data,
    bool local,
    const QString& fallbackId) const
{
    CardData card;
    card.id = fallbackId;
    card.title = QStringLiteral("Untitled experiment");
    card.summary = QStringLiteral("No summary provided.");
    card.local = local;

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        qWarning() << "[MainWindow] failed to parse experiment card:"
            << parseError.errorString();
        return card;
    }

    const QJsonObject root = document.object();
    card.id = root.value(QStringLiteral("id")).toString(card.id).trimmed();
    card.title = root.value(QStringLiteral("title")).toString(card.title).trimmed();
    card.summary = root.value(QStringLiteral("summary")).toString(card.summary).trimmed();

    const QJsonArray tags = root.value(QStringLiteral("tags")).toArray();
    if (!tags.isEmpty()) {
        card.tag = tags.first().toString().trimmed();
    }
    if (card.tag.isEmpty()) {
        card.tag = root.value(QStringLiteral("schema")).toString().trimmed();
    }

    if (card.id.isEmpty()) {
        card.id = fallbackId;
    }
    if (card.title.isEmpty()) {
        card.title = QStringLiteral("Untitled experiment");
    }
    if (card.summary.isEmpty()) {
        card.summary = QStringLiteral("No summary provided.");
    }

    return card;
}


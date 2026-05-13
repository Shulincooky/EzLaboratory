#include "LoginDialog.h"

#include "ui_LoginDialog.h"

#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QRegularExpression>
#include <QStackedWidget>
#include <QTimer>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog())
    , m_emailWarningTimer(new QTimer(this))
    , m_passwordWarningTimer(new QTimer(this))
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->headPortrait->setPixmap(QPixmap(":/Login/images/login/portrait.png").scaled(100, 100));
    ui->headPortrait_2->setPixmap(QPixmap(":/Login/images/login/portrait.png").scaled(100, 100));
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit_2->setEchoMode(QLineEdit::Password);

    setupWarningLabels();

    connect(ui->CloseButton, &QPushButton::released, this, &QDialog::reject);
    connect(ui->MinusButton, &QPushButton::released, this, &QWidget::showMinimized);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::handleLogin);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginDialog::handleRegister);
    connect(ui->registerCut, &QPushButton::clicked, this, [this]() {
        ui->formStack->setCurrentWidget(ui->registerLayout);
    });
    connect(ui->loginCut, &QPushButton::clicked, this, [this]() {
        ui->formStack->setCurrentWidget(ui->loginLayout);
    });
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::email() const
{
    return m_email;
}

QString LoginDialog::nickname() const
{
    return m_nickname;
}

void LoginDialog::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
        return;
    }

    QDialog::mousePressEvent(event);
}

void LoginDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
        return;
    }

    QDialog::mouseMoveEvent(event);
}

void LoginDialog::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(240, 240, 240));
    painter.setPen(QColor(0, 0, 0, 80));
    painter.drawRoundedRect(rect(), 4, 4);
}

void LoginDialog::setupWarningLabels()
{
    ui->emailWarring->setPixmap(QPixmap(":/Login/images/login/email_required.png").scaled(250, 41));
    ui->passwordWarring->setPixmap(QPixmap(":/Login/images/login/password_required.png").scaled(195, 41));
    ui->registerWarring->setText(QStringLiteral("请补全注册信息后继续"));
    addShadowEffect(ui->emailWarring);
    addShadowEffect(ui->passwordWarring);
    ui->emailWarring->hide();
    ui->passwordWarring->hide();
    ui->registerWarring->hide();

    m_emailWarningTimer->setSingleShot(true);
    m_passwordWarningTimer->setSingleShot(true);
    connect(m_emailWarningTimer, &QTimer::timeout, ui->emailWarring, &QWidget::hide);
    connect(m_emailWarningTimer, &QTimer::timeout, ui->registerWarring, &QWidget::hide);
    connect(m_passwordWarningTimer, &QTimer::timeout, ui->passwordWarring, &QWidget::hide);
}

void LoginDialog::showTemporaryWarning(QLabel* label, QTimer* timer)
{
    label->show();
    timer->start(1000);
}

void LoginDialog::addShadowEffect(QLabel* label)
{
    auto* shadow = new QGraphicsDropShadowEffect(label);
    shadow->setBlurRadius(25);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(3, 3);
    label->setGraphicsEffect(shadow);
}

bool LoginDialog::validateEmail(const QString& email) const
{
    static const QRegularExpression regex(
        QStringLiteral(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"));
    return regex.match(email).hasMatch();
}

bool LoginDialog::validatePassword(const QString& password) const
{
    return password.length() >= 6;
}

void LoginDialog::acceptWithAccount(const QString& email, const QString& nickname)
{
    m_email = email;
    m_nickname = nickname;
    accept();
}

void LoginDialog::handleLogin()
{
    ui->emailWarring->setPixmap(QPixmap(":/Login/images/login/email_required.png").scaled(250, 41));
    ui->passwordWarring->setPixmap(QPixmap(":/Login/images/login/password_required.png").scaled(195, 41));
    m_emailWarningTimer->stop();
    m_passwordWarningTimer->stop();
    ui->emailWarring->hide();
    ui->passwordWarring->hide();

    const QString email = ui->emailEdit->text().trimmed();
    const QString password = ui->passwordEdit->text();

    if (!validateEmail(email)) {
        showTemporaryWarning(ui->emailWarring, m_emailWarningTimer);
        return;
    }

    if (!validatePassword(password)) {
        showTemporaryWarning(ui->passwordWarring, m_passwordWarningTimer);
        return;
    }

    acceptWithAccount(email, email.section('@', 0, 0));
}

void LoginDialog::handleRegister()
{
    const QString email = ui->emailEdit_2->text().trimmed();
    const QString password = ui->passwordEdit_2->text();
    const QString nickname = ui->usernameEdit->text().trimmed();

    if (!validateEmail(email) || !validatePassword(password) || nickname.isEmpty()) {
        ui->registerWarring->setText(QStringLiteral("请补全注册信息后继续"));
        showTemporaryWarning(ui->registerWarring, m_emailWarningTimer);
        return;
    }

    acceptWithAccount(email, nickname);
}

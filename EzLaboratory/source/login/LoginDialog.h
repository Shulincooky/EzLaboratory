#pragma once

#include <QDialog>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class QLabel;
class QMouseEvent;
class QPaintEvent;
class QTimer;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog() override;

    QString email() const;
    QString nickname() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void setupWarningLabels();
    void showTemporaryWarning(QLabel* label, QTimer* timer);
    void addShadowEffect(QLabel* label);
    bool validateEmail(const QString& email) const;
    bool validatePassword(const QString& password) const;
    void acceptWithAccount(const QString& email, const QString& nickname);

private slots:
    void handleLogin();
    void handleRegister();

private:
    Ui::LoginDialog* ui = nullptr;
    QTimer* m_emailWarningTimer = nullptr;
    QTimer* m_passwordWarningTimer = nullptr;
    QPoint m_dragPosition;
    QString m_email;
    QString m_nickname;
};

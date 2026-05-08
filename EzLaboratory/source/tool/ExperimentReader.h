#pragma once

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QString>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;

struct ExperimentReadResult
{
    QByteArray data;
    QString origin;
    bool local = true;
};

class ExperimentReader : public QObject
{
    Q_OBJECT

public:
    explicit ExperimentReader(QObject* parent = nullptr);

    QByteArray readLocalFile(const QString& filePath, QString* errorString = nullptr) const;
    QList<ExperimentReadResult> readLocalDirectory(const QString& directoryPath,
        QString* errorString = nullptr) const;

    void readRemotePack(const QUrl& url);

signals:
    void remotePackLoaded(const QUrl& url, const QByteArray& data);
    void remotePackFailed(const QUrl& url, const QString& errorString);

private:
    void handleRemoteReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_network = nullptr;
};

#include "ExperimentReader.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <algorithm>

ExperimentReader::ExperimentReader(QObject* parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
{
    connect(m_network, &QNetworkAccessManager::finished,
        this, &ExperimentReader::handleRemoteReply);
}

QByteArray ExperimentReader::readLocalFile(const QString& filePath, QString* errorString) const
{
    if (errorString) {
        errorString->clear();
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorString) {
            *errorString = QStringLiteral("Failed to open experiment file: %1").arg(filePath);
        }
        return {};
    }

    return file.readAll();
}

QList<ExperimentReadResult> ExperimentReader::readLocalDirectory(
    const QString& directoryPath,
    QString* errorString) const
{
    if (errorString) {
        errorString->clear();
    }

    QDir directory(directoryPath);
    if (!directory.exists()) {
        if (errorString) {
            *errorString = QStringLiteral("Experiment directory does not exist: %1").arg(directoryPath);
        }
        return {};
    }

    QStringList filePaths;
    QDirIterator iterator(
        directoryPath,
        QStringList{ QStringLiteral("*.json") },
        QDir::Files | QDir::Readable,
        QDirIterator::Subdirectories);

    while (iterator.hasNext()) {
        filePaths.push_back(iterator.next());
    }

    std::sort(filePaths.begin(), filePaths.end(), [](const QString& left, const QString& right) {
        return left.compare(right, Qt::CaseInsensitive) < 0;
    });

    QList<ExperimentReadResult> results;
    results.reserve(filePaths.size());

    for (const QString& filePath : filePaths) {
        QString fileError;
        const QByteArray data = readLocalFile(filePath, &fileError);
        if (data.isEmpty() && !fileError.isEmpty()) {
            if (errorString && errorString->isEmpty()) {
                *errorString = fileError;
            }
            continue;
        }

        ExperimentReadResult result;
        result.data = data;
        result.origin = filePath;
        result.local = true;
        results.push_back(result);
    }

    return results;
}

void ExperimentReader::readRemotePack(const QUrl& url)
{
    if (!url.isValid() || url.isEmpty()) {
        emit remotePackFailed(url, QStringLiteral("Invalid remote pack URL."));
        return;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("EzLaboratory"));
    m_network->get(request);
}

void ExperimentReader::handleRemoteReply(QNetworkReply* reply)
{
    const QUrl url = reply->url();

    if (reply->error() != QNetworkReply::NoError) {
        emit remotePackFailed(url, reply->errorString());
        reply->deleteLater();
        return;
    }

    emit remotePackLoaded(url, reply->readAll());
    reply->deleteLater();
}

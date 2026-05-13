#pragma once

#include <QByteArray>
#include <QColor>
#include <QHash>
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QStringList>

struct ExperimentCardInfo
{
    QString id;
    QString title;
    QString summary;
    QString cover;
    QString tag;
    QStringList tags;

    QString searchText() const;
};

struct ExperimentLabelInfo
{
    QString layout;
    QString centerText;
    QString topText;
    QString bottomText;
};

struct ExperimentChemicalAppearance
{
    QString kind;
    QString value;
    QColor color;
};

struct ExperimentChemicalInfo
{
    QString id;
    QString name;
    QString state;
    ExperimentChemicalAppearance appearance;
    double density = 0.0;
    bool hasDensity = false;
};

struct ExperimentLabwareInfo
{
    QString id;
    QString bodyType;
    QString plugType;
    QString name;
    QString style;
    int limit = 0;
    ExperimentLabelInfo label;
    QStringList chemicalIds;
};

struct ExperimentDefinition
{
    QString schema;
    ExperimentCardInfo card;
    QList<ExperimentLabwareInfo> labwares;
    QHash<QString, ExperimentChemicalInfo> chemicals;
    QStringList expressions;
    QByteArray rawJson;
};

struct ExperimentParseResult
{
    bool ok = false;
    ExperimentDefinition experiment;
    QString errorString;
};

class ExperimentParser
{
public:
    ExperimentParseResult parseExperiment(const QByteArray& data) const;
    ExperimentCardInfo parseExperimentCard(const QByteArray& data,
        QString* errorString = nullptr) const;
    ExperimentCardInfo cardInfo(const ExperimentDefinition& experiment) const;

private:
    ExperimentCardInfo parseExperimentCard(const QJsonObject& root) const;
    QList<ExperimentLabwareInfo> parseLabwares(const QJsonObject& root) const;
    QHash<QString, ExperimentChemicalInfo> parseChemicals(const QJsonObject& root) const;
    QStringList parseExpressions(const QJsonObject& root) const;
};

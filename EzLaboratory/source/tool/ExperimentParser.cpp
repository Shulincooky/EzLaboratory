#include "ExperimentParser.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>

namespace
{
    QString objectString(const QJsonObject& object,
        const QString& key,
        const QString& fallback = QString())
    {
        return object.value(key).toString(fallback).trimmed();
    }

    QStringList stringArray(const QJsonArray& array)
    {
        QStringList values;
        values.reserve(array.size());

        for (const QJsonValue& value : array) {
            const QString text = value.toString().trimmed();
            if (!text.isEmpty()) {
                values.push_back(text);
            }
        }

        return values;
    }

    ExperimentChemicalAppearance parseAppearance(const QJsonValue& value)
    {
        ExperimentChemicalAppearance appearance;

        if (value.isString()) {
            appearance.value = value.toString().trimmed();
            appearance.color = QColor(appearance.value);
            appearance.kind = appearance.color.isValid()
                ? QStringLiteral("color")
                : QStringLiteral("literal");
            return appearance;
        }

        if (!value.isObject()) {
            return appearance;
        }

        const QJsonObject object = value.toObject();
        appearance.kind = objectString(object, QStringLiteral("kind"));
        appearance.value = objectString(object, QStringLiteral("value"));
        if (appearance.value.isEmpty()) {
            appearance.value = objectString(object, QStringLiteral("color"));
        }
        appearance.color = QColor(appearance.value);

        if (appearance.kind.isEmpty() && appearance.color.isValid()) {
            appearance.kind = QStringLiteral("color");
        }

        return appearance;
    }
}

QString ExperimentCardInfo::searchText() const
{
    return QStringLiteral("%1\n%2\n%3\n%4\n%5").arg(
        id,
        title,
        summary,
        tag,
        tags.join(QLatin1Char('\n')));
}

ExperimentParseResult ExperimentParser::parseExperiment(const QByteArray& data) const
{
    ExperimentParseResult result;

    if (data.isEmpty()) {
        result.errorString = QStringLiteral("Experiment data is empty.");
        return result;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        result.errorString = QStringLiteral("Failed to parse experiment JSON: %1")
            .arg(parseError.errorString());
        return result;
    }

    if (!document.isObject()) {
        result.errorString = QStringLiteral("Experiment JSON root must be an object.");
        return result;
    }

    const QJsonObject root = document.object();
    ExperimentDefinition experiment;
    experiment.rawJson = data;
    experiment.schema = objectString(root, QStringLiteral("schema"));
    experiment.card = parseExperimentCard(root);
    experiment.labwares = parseLabwares(root);
    experiment.chemicals = parseChemicals(root);
    experiment.expressions = parseExpressions(root);

    if (experiment.card.id.isEmpty()) {
        result.errorString = QStringLiteral("Experiment id is empty.");
        return result;
    }

    result.ok = true;
    result.experiment = experiment;
    return result;
}

ExperimentCardInfo ExperimentParser::parseExperimentCard(
    const QByteArray& data,
    QString* errorString) const
{
    if (errorString) {
        errorString->clear();
    }

    if (data.isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("Experiment data is empty.");
        }
        return {};
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (errorString) {
            *errorString = QStringLiteral("Failed to parse experiment JSON: %1")
                .arg(parseError.errorString());
        }
        return {};
    }

    if (!document.isObject()) {
        if (errorString) {
            *errorString = QStringLiteral("Experiment JSON root must be an object.");
        }
        return {};
    }

    return parseExperimentCard(document.object());
}

ExperimentCardInfo ExperimentParser::cardInfo(const ExperimentDefinition& experiment) const
{
    return experiment.card;
}

ExperimentCardInfo ExperimentParser::parseExperimentCard(const QJsonObject& root) const
{
    ExperimentCardInfo cardInfo;
    cardInfo.id = objectString(root, QStringLiteral("id"));
    cardInfo.title = objectString(root, QStringLiteral("title"),
        QStringLiteral("Untitled experiment"));
    cardInfo.summary = objectString(root, QStringLiteral("summary"),
        QStringLiteral("No summary provided."));
    cardInfo.cover = objectString(root, QStringLiteral("cover"));
    cardInfo.tags = stringArray(root.value(QStringLiteral("tags")).toArray());

    if (!cardInfo.tags.isEmpty()) {
        cardInfo.tag = cardInfo.tags.first();
    }
    if (cardInfo.tag.isEmpty()) {
        cardInfo.tag = objectString(root, QStringLiteral("schema"));
    }
    if (cardInfo.title.isEmpty()) {
        cardInfo.title = QStringLiteral("Untitled experiment");
    }
    if (cardInfo.summary.isEmpty()) {
        cardInfo.summary = QStringLiteral("No summary provided.");
    }

    return cardInfo;
}

QList<ExperimentLabwareInfo> ExperimentParser::parseLabwares(const QJsonObject& root) const
{
    const QJsonArray labwareArray = root.value(QStringLiteral("labwares")).toArray();
    QList<ExperimentLabwareInfo> labwares;
    labwares.reserve(labwareArray.size());

    for (const QJsonValue& value : labwareArray) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject object = value.toObject();
        const QJsonObject type = object.value(QStringLiteral("type")).toObject();
        const QJsonObject data = object.value(QStringLiteral("data")).toObject();
        const QJsonObject label = data.value(QStringLiteral("label")).toObject();

        ExperimentLabwareInfo labware;
        labware.id = objectString(object, QStringLiteral("id"));
        labware.bodyType = objectString(type, QStringLiteral("body"));
        labware.plugType = objectString(type, QStringLiteral("plug"));
        labware.name = objectString(object, QStringLiteral("name"), labware.id);
        labware.limit = object.value(QStringLiteral("limit")).toInt(0);
        labware.style = objectString(data, QStringLiteral("style"));
        labware.chemicalIds = stringArray(data.value(QStringLiteral("chemicals")).toArray());
        labware.label.layout = objectString(label, QStringLiteral("layout"));
        labware.label.centerText = objectString(label, QStringLiteral("centerText"));
        labware.label.topText = objectString(label, QStringLiteral("topText"));
        labware.label.bottomText = objectString(label, QStringLiteral("bottomText"));

        if (labware.name.isEmpty()) {
            labware.name = labware.bodyType;
        }

        labwares.push_back(labware);
    }

    return labwares;
}

QHash<QString, ExperimentChemicalInfo> ExperimentParser::parseChemicals(
    const QJsonObject& root) const
{
    const QJsonArray chemicalArray = root.value(QStringLiteral("chemicals")).toArray();
    QHash<QString, ExperimentChemicalInfo> chemicals;
    chemicals.reserve(chemicalArray.size());

    for (const QJsonValue& value : chemicalArray) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject object = value.toObject();
        ExperimentChemicalInfo chemical;
        chemical.id = objectString(object, QStringLiteral("id"));
        chemical.name = objectString(object, QStringLiteral("name"), chemical.id);
        chemical.state = objectString(object, QStringLiteral("state"));

        chemical.appearance = parseAppearance(object.value(QStringLiteral("appearance")));

        const QJsonValue density = object.value(QStringLiteral("density"));
        chemical.hasDensity = density.isDouble();
        chemical.density = density.toDouble(0.0);

        if (!chemical.id.isEmpty()) {
            chemicals.insert(chemical.id, chemical);
        }
    }

    return chemicals;
}

QStringList ExperimentParser::parseExpressions(const QJsonObject& root) const
{
    return stringArray(root.value(QStringLiteral("expressions")).toArray());
}

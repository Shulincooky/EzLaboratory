#pragma once

#pragma once

#include <QByteArray>
#include <QColor>
#include <QHash>
#include <QJsonArray>
#include <QList>
#include <QString>
#include <QStringList>

struct ChemicalDefinition
{
    QString id;
    QString alias;
    QString displayName;
    QString phase;               // "liquid" / "solid"
    QColor liquidColor;
    QString solidTexturePath;
};

struct SidebarBottleTemplate
{
    QString containerType;       // "narrow_bottle" / "wide_bottle"
    QString displayName;

    QString centerText;
    QString topText;
    QString bottomText;

    bool enableLiquid = false;
    QColor liquidColor;

    bool enableSolid = false;
    QString solidTexturePath;
    qreal solidFillRatio = 0.65;

    QStringList chemicalIds;
    int limit = 1;
};

class ExperimentSidebarConfigLoader
{
public:
    bool loadFromFile(const QString& filePath);
    QString errorString() const;

    QList<SidebarBottleTemplate> bottleTemplates() const;

private:
    bool parseRoot(const QByteArray& jsonData);
    bool parseChemicals(const QJsonArray& array);
    bool parseSidebar(const QJsonArray& array);

    QString resolvedChemicalDisplayName(const ChemicalDefinition& chemical) const;
    QString resolvedBottleDisplayName(const ChemicalDefinition& chemical, const QString& containerType) const;
    QString resolvedContainerType(const QString& requested, const ChemicalDefinition& chemical) const;

private:
    QString m_errorString;
    QHash<QString, ChemicalDefinition> m_chemicals;
    QList<SidebarBottleTemplate> m_bottleTemplates;
};
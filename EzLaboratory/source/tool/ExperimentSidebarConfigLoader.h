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
    QString phase;               // currently only "liquid" is supported
    QColor liquidColor;
};

struct SidebarTemplate
{
    QString type;                // "beaker" / "chemical_container"
    QString containerType;       // "narrow_bottle" / "wide_bottle"
    QString displayName;
    QString iconPath;

    QString centerText;
    QString topText;
    QString bottomText;

    bool enableLiquid = false;
    QColor liquidColor;

    QStringList chemicalIds;
    int limit = 1;
};

struct ReactionTemplate
{
    QString equation;
    QStringList reactantIds;

    QString productLiquidId;
    QColor productLiquidColor;
};

class ExperimentSidebarConfigLoader
{
public:
    bool loadFromFile(const QString& filePath);
    QString errorString() const;

    QList<SidebarTemplate> sidebarTemplates() const;
    QList<ReactionTemplate> reactionTemplates() const;

private:
    bool parseRoot(const QByteArray& jsonData);
    bool parseChemicals(const QJsonArray& array);
    bool parseSidebar(const QJsonArray& array);
    bool parseReactions(const QJsonArray& array);

    QString resolvedChemicalDisplayName(const ChemicalDefinition& chemical) const;
    QString resolvedBottleDisplayName(const ChemicalDefinition& chemical, const QString& containerType) const;
    QString resolvedContainerType(const QString& requested) const;

private:
    QString m_errorString;
    QHash<QString, ChemicalDefinition> m_chemicals;
    QList<SidebarTemplate> m_sidebarTemplates;
    QList<ReactionTemplate> m_reactionTemplates;
};

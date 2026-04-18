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

struct SidebarTemplate
{
    QString type;                // "beaker" / "tweezers" / "chemical_container"
    QString containerType;       // "narrow_bottle" / "wide_bottle"，仅 chemical_container 使用
    QString displayName;
    QString iconPath;

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

struct ReactionTemplate
{
    QString equation;
    QStringList reactantIds;

    QString productLiquidId;
    QColor productLiquidColor;

    QString productSolidId;
    QString productSolidTexturePath;
    qreal productSolidFillRatio = 0.35;
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
    QString resolvedContainerType(const QString& requested, const ChemicalDefinition& chemical) const;

private:
    QString m_errorString;
    QHash<QString, ChemicalDefinition> m_chemicals;
    QList<SidebarTemplate> m_sidebarTemplates;
    QList<ReactionTemplate> m_reactionTemplates;
}
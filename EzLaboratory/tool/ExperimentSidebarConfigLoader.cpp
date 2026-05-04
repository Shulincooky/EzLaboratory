#include "ExperimentSidebarConfigLoader.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

bool ExperimentSidebarConfigLoader::loadFromFile(const QString& filePath)
{
    m_errorString.clear();
    m_chemicals.clear();
    m_sidebarTemplates.clear();
    m_reactionTemplates.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_errorString = QStringLiteral("无法打开配置文件: %1").arg(filePath);
        return false;
    }

    return parseRoot(file.readAll());
}

QString ExperimentSidebarConfigLoader::errorString() const
{
    return m_errorString;
}

QList<SidebarTemplate> ExperimentSidebarConfigLoader::sidebarTemplates() const
{
    return m_sidebarTemplates;
}

QList<ReactionTemplate> ExperimentSidebarConfigLoader::reactionTemplates() const
{
    return m_reactionTemplates;
}

bool ExperimentSidebarConfigLoader::parseRoot(const QByteArray& jsonData)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        m_errorString = QStringLiteral("JSON 解析失败: %1").arg(parseError.errorString());
        return false;
    }

    const QJsonObject root = doc.object();

    if (!root.contains("chemicals") || !root.value("chemicals").isArray()) {
        m_errorString = QStringLiteral("配置文件缺少 chemicals 数组");
        return false;
    }

    if (!root.contains("sidebar") || !root.value("sidebar").isArray()) {
        m_errorString = QStringLiteral("配置文件缺少 sidebar 数组");
        return false;
    }

    if (!parseChemicals(root.value("chemicals").toArray())) {
        return false;
    }

    if (!parseSidebar(root.value("sidebar").toArray())) {
        return false;
    }

    if (root.contains("reactions")) {
        if (!root.value("reactions").isArray()) {
            m_errorString = QStringLiteral("配置文件中的 reactions 必须是数组");
            return false;
        }

        if (!parseReactions(root.value("reactions").toArray())) {
            return false;
        }
    }

    return true;
}

bool ExperimentSidebarConfigLoader::parseChemicals(const QJsonArray& array)
{
    for (const QJsonValue& value : array) {
        if (!value.isObject()) {
            m_errorString = QStringLiteral("chemicals 数组中存在非对象项");
            return false;
        }

        const QJsonObject obj = value.toObject();

        const QString id = obj.value("id").toString().trimmed();
        if (id.isEmpty()) {
            m_errorString = QStringLiteral("chemical 缺少 id");
            return false;
        }

        ChemicalDefinition chemical;
        chemical.id = id;
        chemical.alias = obj.value("alias").toString().trimmed();
        chemical.displayName = obj.value("displayName").toString().trimmed();
        chemical.phase = obj.value("phase").toString().trimmed().toLower();
        chemical.liquidColor = QColor(obj.value("liquidColor").toString().trimmed());
        chemical.solidTexturePath = obj.value("solidTexturePath").toString().trimmed();

        if (chemical.phase != "liquid" && chemical.phase != "solid") {
            m_errorString = QStringLiteral("chemical[%1] 的 phase 只能是 liquid 或 solid").arg(id);
            return false;
        }

        m_chemicals.insert(id, chemical);
    }

    return true;
}

bool ExperimentSidebarConfigLoader::parseSidebar(const QJsonArray& array)
{
    for (const QJsonValue& value : array) {
        if (!value.isObject()) {
            m_errorString = QStringLiteral("sidebar 数组中存在非对象项");
            return false;
        }

        const QJsonObject obj = value.toObject();

        QString entryType = obj.value("type").toString().trimmed().toLower();
        if (entryType.isEmpty()) {
            entryType = QStringLiteral("chemical_container");
        }

        SidebarTemplate item;
        item.type = entryType;

        const int limit = obj.contains("limit") ? obj.value("limit").toInt(1) : 1;
        item.limit = qMax(1, limit);

        if (entryType == "beaker") {
            item.displayName = obj.value("displayName").toString().trimmed();
            if (item.displayName.isEmpty()) {
                item.displayName = QStringLiteral("烧杯");
            }
            item.iconPath = QStringLiteral(":/EzLaboratory/resources/image/glassware/beaker.png");
            m_sidebarTemplates.push_back(item);
            continue;
        }

        if (entryType == "tweezers") {
            item.displayName = obj.value("displayName").toString().trimmed();
            if (item.displayName.isEmpty()) {
                item.displayName = QStringLiteral("镊子");
            }
            item.iconPath = QStringLiteral(":/EzLaboratory/resources/image/lab_tools/tweezers.png");
            m_sidebarTemplates.push_back(item);
            continue;
        }

        if (entryType != "chemical_container") {
            m_errorString = QStringLiteral("不支持的 sidebar.type: %1").arg(entryType);
            return false;
        }

        const QString chemicalId = obj.value("chemicalId").toString().trimmed();
        if (chemicalId.isEmpty()) {
            m_errorString = QStringLiteral("chemical_container 缺少 chemicalId");
            return false;
        }

        if (!m_chemicals.contains(chemicalId)) {
            m_errorString = QStringLiteral("sidebar 引用了未定义 chemicalId: %1").arg(chemicalId);
            return false;
        }

        const ChemicalDefinition chemical = m_chemicals.value(chemicalId);

        const QString requestedContainer =
            obj.value("container").toString("auto").trimmed().toLower();
        const QString containerType = resolvedContainerType(requestedContainer, chemical);
        if (containerType.isEmpty()) {
            m_errorString = QStringLiteral("chemical[%1] 的 container 无法解析").arg(chemicalId);
            return false;
        }

        item.containerType = containerType;
        item.chemicalIds = QStringList{ chemicalId };

        const QString displayName =
            obj.value("displayName").toString().trimmed();
        item.displayName = displayName.isEmpty()
            ? resolvedBottleDisplayName(chemical, containerType)
            : displayName;

        const QJsonObject labelObj = obj.value("label").toObject();
        item.centerText = labelObj.value("center").toString().trimmed();
        item.topText = labelObj.value("top").toString().trimmed();
        item.bottomText = labelObj.value("bottom").toString().trimmed();

        if (item.centerText.isEmpty() && item.topText.isEmpty() && item.bottomText.isEmpty()) {
            item.centerText = resolvedChemicalDisplayName(chemical);
        }

        if (chemical.phase == "liquid") {
            item.enableLiquid = true;
            item.liquidColor = chemical.liquidColor.isValid()
                ? chemical.liquidColor
                : QColor(90, 150, 255, 80);
        }
        else if (chemical.phase == "solid") {
            item.enableSolid = true;
            item.solidTexturePath = chemical.solidTexturePath;
            item.solidFillRatio = obj.contains("solidFillRatio")
                ? qBound(0.05, obj.value("solidFillRatio").toDouble(0.65), 1.0)
                : 0.65;

            if (item.solidTexturePath.isEmpty()) {
                m_errorString = QStringLiteral("solid chemical[%1] 缺少 solidTexturePath").arg(chemicalId);
                return false;
            }
        }

        m_sidebarTemplates.push_back(item);
    }

    return true;
}

bool ExperimentSidebarConfigLoader::parseReactions(const QJsonArray& array)
{
    for (const QJsonValue& value : array) {
        if (!value.isObject()) {
            m_errorString = QStringLiteral("reactions 数组中存在非对象项");
            return false;
        }

        const QJsonObject obj = value.toObject();

        ReactionTemplate reaction;
        reaction.equation = obj.value("equation").toString().trimmed();

        const QJsonArray reactantsArray = obj.value("reactants").toArray();
        for (const QJsonValue& reactantValue : reactantsArray) {
            const QString id = reactantValue.toString().trimmed();
            if (id.isEmpty()) {
                continue;
            }
            if (!m_chemicals.contains(id)) {
                m_errorString = QStringLiteral("reaction 引用了未定义反应物: %1").arg(id);
                return false;
            }
            reaction.reactantIds.push_back(id);
        }

        if (reaction.reactantIds.isEmpty()) {
            m_errorString = QStringLiteral("reaction 缺少 reactants");
            return false;
        }

        reaction.productLiquidId = obj.value("productLiquidId").toString().trimmed();
        if (!reaction.productLiquidId.isEmpty()) {
            if (!m_chemicals.contains(reaction.productLiquidId)) {
                m_errorString = QStringLiteral("reaction 引用了未定义液体产物: %1").arg(reaction.productLiquidId);
                return false;
            }
            reaction.productLiquidColor = m_chemicals.value(reaction.productLiquidId).liquidColor;
        }

        reaction.productSolidId = obj.value("productSolidId").toString().trimmed();
        if (!reaction.productSolidId.isEmpty()) {
            if (!m_chemicals.contains(reaction.productSolidId)) {
                m_errorString = QStringLiteral("reaction 引用了未定义固体产物: %1").arg(reaction.productSolidId);
                return false;
            }
            reaction.productSolidTexturePath = m_chemicals.value(reaction.productSolidId).solidTexturePath;
            reaction.productSolidFillRatio = obj.contains("productSolidFillRatio")
                ? qBound(0.05, obj.value("productSolidFillRatio").toDouble(0.35), 1.0)
                : 0.35;
        }

        m_reactionTemplates.push_back(reaction);
    }

    return true;
}

QString ExperimentSidebarConfigLoader::resolvedChemicalDisplayName(const ChemicalDefinition& chemical) const
{
    if (!chemical.displayName.isEmpty()) {
        return chemical.displayName;
    }
    if (!chemical.alias.isEmpty()) {
        return chemical.alias;
    }
    return chemical.id;
}

QString ExperimentSidebarConfigLoader::resolvedBottleDisplayName(const ChemicalDefinition& chemical, const QString& containerType) const
{
    const QString base = resolvedChemicalDisplayName(chemical);
    if (containerType == "wide_bottle") {
        return base + QStringLiteral("广口瓶");
    }
    if (containerType == "narrow_bottle") {
        return base + QStringLiteral("细口瓶");
    }
    return base;
}

QString ExperimentSidebarConfigLoader::resolvedContainerType(const QString& requested, const ChemicalDefinition& chemical) const
{
    if (requested == "wide_bottle" || requested == "narrow_bottle") {
        return requested;
    }

    if (requested == "auto" || requested.isEmpty()) {
        if (chemical.phase == "solid") {
            return "wide_bottle";
        }
        if (chemical.phase == "liquid") {
            return "narrow_bottle";
        }
    }

    return QString();
}
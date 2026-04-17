#pragma once
#include <Qt>

namespace LabwareRoles {
    constexpr int Type = Qt::UserRole + 1;
    constexpr int Limit = Qt::UserRole + 2;
    constexpr int Remaining = Qt::UserRole + 3;
    constexpr int LabelLayout = Qt::UserRole + 4;
    constexpr int LabelCenterText = Qt::UserRole + 5;
    constexpr int LabelTopText = Qt::UserRole + 6;
    constexpr int LabelBottomText = Qt::UserRole + 7;
    constexpr int TemplateId = Qt::UserRole + 8;
    constexpr int LiquidEnabled = Qt::UserRole + 9;
    constexpr int LiquidColor = Qt::UserRole + 10;
}
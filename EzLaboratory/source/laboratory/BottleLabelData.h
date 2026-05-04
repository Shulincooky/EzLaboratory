#pragma once

#include <QString>

enum class BottleLabelLayout
{
    SingleCenter,
    DoubleLine
};

struct BottleLabelData
{
    BottleLabelLayout layout = BottleLabelLayout::SingleCenter;

    // 单行式：只使用 centerText
    QString centerText;

    // 二段式：只使用 topText + bottomText
    QString topText;
    QString bottomText;
};
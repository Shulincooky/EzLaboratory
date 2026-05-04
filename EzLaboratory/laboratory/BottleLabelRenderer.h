#pragma once

#include <QPixmap>
#include <QSize>
#include <QString>

#include "BottleLabelData.h"

class BottleLabelRenderer
{
public:
    static QPixmap renderLabel(const BottleLabelData& data,
        const QSize& pixelSize,
        const QString& templatePath);
};
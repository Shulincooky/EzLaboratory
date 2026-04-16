#pragma once

#include <QGraphicsPixmapItem>
#include <QSizeF>
#include <QString>

#include "BottleLabelData.h"

class BottleLabelItem : public QGraphicsPixmapItem
{
public:
    explicit BottleLabelItem(QGraphicsItem* parent = nullptr);

    void updateLabel(const BottleLabelData& data,
        const QSizeF& logicalSize,
        const QString& templatePath);
};
#include "BottleLabelRenderer.h"

#include <QFile>
#include <QPainter>
#include <QtSvg/QSvgRenderer>
#include <QByteArray>

namespace
{
    QByteArray buildSvgData(const QByteArray& originalSvg, const BottleLabelData& data)
    {
        QByteArray svg = originalSvg;

        // 模板中的三个占位词，按你给的 svg 来替换：
        // 上：HCL
        // 中：中心占位
        // 下：4mol/L

        if (data.layout == BottleLabelLayout::SingleCenter) {
            svg.replace(">HCL<", "> <");
            svg.replace(">中心占位<", ">" + data.centerText.toUtf8() + "<");
            svg.replace(">4mol/L<", "> <");
        }
        else { // DoubleLine
            svg.replace(">HCL<", ">" + data.topText.toUtf8() + "<");
            svg.replace(">中心占位<", "> <");
            svg.replace(">4mol/L<", ">" + data.bottomText.toUtf8() + "<");
        }

        return svg;
    }
}

QPixmap BottleLabelRenderer::renderLabel(const BottleLabelData& data,
    const QSize& pixelSize,
    const QString& templatePath)
{
    QFile file(templatePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QPixmap();
    }

    const QByteArray originalSvg = file.readAll();
    const QByteArray finalSvg = buildSvgData(originalSvg, data);

    QSvgRenderer renderer(finalSvg);
    if (!renderer.isValid()) {
        return QPixmap();
    }

    QPixmap pixmap(pixelSize);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    renderer.render(&painter);

    return pixmap;
}
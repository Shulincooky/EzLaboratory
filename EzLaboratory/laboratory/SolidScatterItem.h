#pragma once

#include <QGraphicsItem>
#include <QPainterPath>
#include <QPixmap>
#include <QRectF>
#include <QString>
#include <QVector>

class SolidScatterItem : public QGraphicsItem
{
public:
    explicit SolidScatterItem(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    void setContainerRect(const QRectF& rect);
    void setClipPath(const QPainterPath& path);

    void setTexturePath(const QString& texturePath);
    QString texturePath() const;

    void setFillRatio(qreal ratio);
    qreal fillRatio() const;

    void setRandomSeed(quint32 seed);
    quint32 randomSeed() const;

private:
    struct Instance
    {
        QRectF targetRect;
        qreal rotationDeg = 0.0;
    };

private:
    void rebuildInstances();

private:
    QRectF m_containerRect;
    QPainterPath m_clipPath;

    QString m_texturePath;
    QPixmap m_texture;

    qreal m_fillRatio = 0.0;
    quint32 m_randomSeed = 0;

    QVector<Instance> m_instances;
};
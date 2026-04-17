#pragma once

#include <QGraphicsObject>
#include <QPixmap>
#include <QString>

class BeakerPourHandleItem : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit BeakerPourHandleItem(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    void setTrackGeometry(qreal x, qreal topY, qreal bottomY);
    void setRatio(qreal ratio);
    qreal ratio() const;

    void setIconPath(const QString& iconPath);
    void setTrackVisible(bool visible);

signals:
    void dragStarted();
    void dragFinished();
    void ratioChanged(qreal ratio);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void updatePosFromRatio();

private:
    qreal m_trackX = 0.0;
    qreal m_trackTopY = 0.0;
    qreal m_trackBottomY = 0.0;
    qreal m_ratio = 0.0; // 0 = bottom, 1 = top

    qreal m_size = 18.0;
    QPixmap m_icon;
    bool m_trackVisible = false;
};
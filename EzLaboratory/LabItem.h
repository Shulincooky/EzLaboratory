#pragma once

#include <QGraphicsObject>
#include <QPixmap>
#include <QString>

class LabItem : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit LabItem(const QString& itemName,
        const QString& imagePath,
        const QSizeF& itemSize,
        QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    QString itemName() const;
    QString imagePath() const;

    void setItemPixmap(const QString& imagePath);
    void setItemSize(const QSizeF& size);
    QSizeF itemSize() const;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    QString m_itemName;
    QString m_imagePath;
    QPixmap m_pixmap;
    QSizeF m_itemSize;
};
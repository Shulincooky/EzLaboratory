#pragma once

#include <QGraphicsView>
#include <QMouseEvent>

class LabGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit LabGraphicsView(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    bool m_isPanning = false;
};
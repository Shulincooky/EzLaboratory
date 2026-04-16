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

private:
    bool m_isPanning = false;
    Qt::MouseButton m_panButton = Qt::NoButton;
};
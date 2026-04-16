#pragma once

#include <QListView>
#include <QResizeEvent>
#include <QMouseEvent>
class LabwareListView : public QListView
{
    Q_OBJECT

public:
    explicit LabwareListView(QWidget* parent = nullptr);

protected:
    void startDrag(Qt::DropActions supportedActions) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};
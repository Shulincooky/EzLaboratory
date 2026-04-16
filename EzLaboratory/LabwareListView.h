#pragma once

#include <QListView>

class LabwareListView : public QListView
{
    Q_OBJECT

public:
    explicit LabwareListView(QWidget* parent = nullptr);

protected:
    void startDrag(Qt::DropActions supportedActions) override;
};
#include "LabwareListView.h"
#include "LabwareItemDelegate.h"

#include <QDrag>
#include <QMimeData>
#include <QModelIndex>
#include <QTimer>
#include <QItemSelectionModel>


namespace
{
    constexpr const char* kLabwareMimeType = "application/x-ezlaboratory-labware";
    constexpr int LabwareTypeRole = Qt::UserRole + 1;
    constexpr int LabwareLimitRole = Qt::UserRole + 2;
	constexpr int LabwareRemainingRole = Qt::UserRole + 3;
}

LabwareListView::LabwareListView(QWidget* parent)
    : QListView(parent)
{
    setDragEnabled(true);
    setAcceptDrops(false);
    setDropIndicatorShown(false);
    setDragDropMode(QAbstractItemView::DragOnly);
    setDefaultDropAction(Qt::CopyAction);
}

void LabwareListView::startDrag(Qt::DropActions)
{
    QModelIndex index = currentIndex();
    if (!index.isValid())
        return;

    const QString type = index.data(LabwareTypeRole).toString();
    const int limit = index.data(LabwareLimitRole).toInt();
    const int remaining = index.data(LabwareRemainingRole).toInt();

    if (type.isEmpty())
        return;
    if (limit > 0 && remaining <= 0)
        return;

    QByteArray payload;
    payload.append(type.toUtf8());
    payload.append('|');
    payload.append(QByteArray::number(limit));
    payload.append('|');
    payload.append(QByteArray::number(remaining));

    auto* mimeData = new QMimeData();
    mimeData->setData(kLabwareMimeType, payload);

    auto* drag = new QDrag(this);
    drag->setMimeData(mimeData);

    const QVariant iconData = index.data(Qt::DecorationRole);
    if (iconData.canConvert<QIcon>()) {
        QIcon icon = qvariant_cast<QIcon>(iconData);
        drag->setPixmap(icon.pixmap(64, 64));
    }

    drag->exec(Qt::CopyAction);

    QTimer::singleShot(0, this, [this]() {
        clearSelection();
        setCurrentIndex(QModelIndex());

        if (selectionModel()) {
            selectionModel()->clear();
        }

        viewport()->update();
        });
}
void LabwareListView::resizeEvent(QResizeEvent* event)
{
    QListView::resizeEvent(event);

    auto* delegate = qobject_cast<LabwareItemDelegate*>(itemDelegate());
    if (delegate) {
        delegate->setViewWidth(viewport()->width());
    }

    doItemsLayout();
    viewport()->update();
}
void LabwareListView::mouseReleaseEvent(QMouseEvent* event)
{
    QListView::mouseReleaseEvent(event);

    QTimer::singleShot(0, this, [this]() {
        clearSelection();
        setCurrentIndex(QModelIndex());

        if (selectionModel()) {
            selectionModel()->clear();
        }

        viewport()->update();
        });
}
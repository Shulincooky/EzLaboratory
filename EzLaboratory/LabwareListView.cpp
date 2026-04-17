#include "LabwareListView.h"
#include "LabwareItemDelegate.h"

#include <QDrag>
#include <QMimeData>
#include <QModelIndex>


namespace
{
    constexpr const char* kLabwareMimeType = "application/x-ezlaboratory-labware";
    constexpr int LabwareTypeRole = Qt::UserRole + 1;
    constexpr int LabwareLimitRole = Qt::UserRole + 2;
	constexpr int LabwareRemainingRole = Qt::UserRole + 3;
    constexpr int LabelLayoutRole = Qt::UserRole + 4;
    constexpr int LabelCenterTextRole = Qt::UserRole + 5;
    constexpr int LabelTopTextRole = Qt::UserRole + 6;
    constexpr int LabelBottomTextRole = Qt::UserRole + 7;
    constexpr int LabwareTemplateIdRole = Qt::UserRole + 8;
    constexpr int LiquidEnabledRole = Qt::UserRole + 9;
    constexpr int LiquidColorRole = Qt::UserRole + 10;
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
    const QString templateId = index.data(LabwareTemplateIdRole).toString();

    const int layout = index.data(LabelLayoutRole).toInt();
    const QString centerText = index.data(LabelCenterTextRole).toString();
    const QString topText = index.data(LabelTopTextRole).toString();
    const QString bottomText = index.data(LabelBottomTextRole).toString();

    const bool liquidEnabled = index.data(LiquidEnabledRole).toBool();
    const QColor liquidColor = index.data(LiquidColorRole).value<QColor>();

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
    payload.append('|');
    payload.append(QByteArray::number(layout));
    payload.append('|');
    payload.append(centerText.toUtf8().toBase64());
    payload.append('|');
    payload.append(topText.toUtf8().toBase64());
    payload.append('|');
    payload.append(bottomText.toUtf8().toBase64());
    payload.append('|');
    payload.append(templateId.toUtf8().toBase64());
    payload.append('|');
    payload.append(liquidEnabled ? '1' : '0');
    payload.append('|');
    payload.append(liquidColor.name(QColor::HexArgb).toUtf8().toBase64());

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
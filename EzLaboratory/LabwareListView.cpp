#include "LabwareListView.h"

#include "LabwareItemDelegate.h"
#include "LabwareTemplateRoles.h"

#include <QDrag>
#include <QMimeData>
#include <QResizeEvent>

namespace {
    constexpr const char* kLabwareMimeType = "application/x-ezlaboratory-labware";
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

    const QString type = index.data(LabwareRoles::Type).toString();
    const int limit = index.data(LabwareRoles::Limit).toInt();
    const int remaining = index.data(LabwareRoles::Remaining).toInt();
    const QString templateId = index.data(LabwareRoles::TemplateId).toString();

    const int layout = index.data(LabwareRoles::LabelLayout).toInt();
    const QString centerText = index.data(LabwareRoles::LabelCenterText).toString();
    const QString topText = index.data(LabwareRoles::LabelTopText).toString();
    const QString bottomText = index.data(LabwareRoles::LabelBottomText).toString();

    const bool liquidEnabled = index.data(LabwareRoles::LiquidEnabled).toBool();
    const QColor liquidColor = index.data(LabwareRoles::LiquidColor).value<QColor>();

    const bool solidEnabled = index.data(LabwareRoles::SolidEnabled).toBool();
    const QString solidTexturePath = index.data(LabwareRoles::SolidTexturePath).toString();
    const qreal solidFillRatio = index.data(LabwareRoles::SolidFillRatio).toDouble();
    const QStringList chemicalIds = index.data(LabwareRoles::ChemicalIds).toStringList();

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
    payload.append('|');
    payload.append(solidEnabled ? '1' : '0');
    payload.append('|');
    payload.append(solidTexturePath.toUtf8().toBase64());
    payload.append('|');
    payload.append(QByteArray::number(solidFillRatio, 'f', 4));
    payload.append('|');
    payload.append(chemicalIds.join('\n').toUtf8().toBase64());

    auto* mimeData = new QMimeData();
    mimeData->setData(kLabwareMimeType, payload);

    auto* drag = new QDrag(this);
    drag->setMimeData(mimeData);

    const QVariant iconData = index.data(Qt::DecorationRole);
    if (iconData.canConvert<QIcon>()) {
        const QIcon icon = qvariant_cast<QIcon>(iconData);
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
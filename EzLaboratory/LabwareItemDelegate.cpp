#include "LabwareItemDelegate.h"

#include <QPainter>
#include <QApplication>

namespace
{
    constexpr int LabwareTypeRole = Qt::UserRole + 1;
    constexpr int LabwareLimitRole = Qt::UserRole + 2;
}

LabwareItemDelegate::LabwareItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void LabwareItemDelegate::paint(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QRect rect = option.rect.adjusted(6, 4, -6, -4);

    // 背景色
    QColor bgColor = Qt::white;
    QColor borderColor = QColor(220, 220, 220);

    if (option.state & QStyle::State_MouseOver) {
        bgColor = QColor(245, 249, 255);
        borderColor = QColor(180, 210, 255);
    }

    if (option.state & QStyle::State_Selected) {
        bgColor = QColor(235, 244, 255);
        borderColor = QColor(100, 160, 255);
    }

    painter->setPen(QPen(borderColor, 1));
    painter->setBrush(bgColor);
    painter->drawRoundedRect(rect, 10, 10);

    // 图标区域
    QRect iconRect(rect.left() + 10, rect.top() + 10, 48, 48);

    QVariant iconData = index.data(Qt::DecorationRole);
    if (iconData.canConvert<QIcon>()) {
        QIcon icon = qvariant_cast<QIcon>(iconData);
        icon.paint(painter, iconRect);
    }

    // 文字区域
    QRect titleRect(iconRect.right() + 10, rect.top() + 10, rect.width() - 80, 24);
    QRect subRect(iconRect.right() + 10, rect.top() + 34, rect.width() - 80, 20);

    QString title = index.data(Qt::DisplayRole).toString();
    int limit = index.data(LabwareLimitRole).toInt();

    QFont titleFont = QApplication::font();
    titleFont.setPointSize(10);
    titleFont.setBold(true);

    QFont subFont = QApplication::font();
    subFont.setPointSize(8);

    painter->setFont(titleFont);
    painter->setPen(QColor(40, 40, 40));
    painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, title);

    painter->setFont(subFont);
    painter->setPen(QColor(130, 130, 130));

    QString subText;
    if (limit > 0)
        subText = QStringLiteral("最多 %1 个").arg(limit);
    else
        subText = QStringLiteral("数量不限");

    painter->drawText(subRect, Qt::AlignLeft | Qt::AlignVCenter, subText);

    painter->restore();
}

QSize LabwareItemDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
{
    return QSize(200, 72);
}
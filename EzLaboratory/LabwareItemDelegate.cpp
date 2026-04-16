#include "LabwareItemDelegate.h"

#include <QPainter>
#include <QApplication>

namespace
{
    constexpr int LabwareTypeRole = Qt::UserRole + 1;
    constexpr int LabwareLimitRole = Qt::UserRole + 2;
    constexpr int LabwareRemainingRole = Qt::UserRole + 3;

    constexpr int kBaseItemWidth = 200;
    constexpr int kBaseItemHeight = 72;

    constexpr int kBaseOuterMarginX = 6;
    constexpr int kBaseOuterMarginY = 4;
    constexpr int kBaseInnerPadding = 10;
    constexpr int kBaseIconSize = 48;
    constexpr int kBaseSpacing = 10;
    constexpr int kBaseTitleHeight = 24;
    constexpr int kBaseSubHeight = 20;
    constexpr int kBaseRadius = 10;
    constexpr int kBaseTitlePointSize = 10;
    constexpr int kBaseSubPointSize = 8;
}

LabwareItemDelegate::LabwareItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}
void LabwareItemDelegate::setViewWidth(int width)
{
    m_viewWidth = qMax(1, width);
}

void LabwareItemDelegate::paint(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const int viewWidth = m_viewWidth;
    const qreal scale = qMax<qreal>(0.6, qreal(viewWidth) / qreal(kBaseItemWidth));

    const int outerMarginX = qRound(kBaseOuterMarginX * scale);
    const int outerMarginY = qRound(kBaseOuterMarginY * scale);
    const int innerPadding = qRound(kBaseInnerPadding * scale);
    const int iconSize = qRound(kBaseIconSize * scale);
    const int spacing = qRound(kBaseSpacing * scale);
    const int titleHeight = qRound(kBaseTitleHeight * scale);
    const int subHeight = qRound(kBaseSubHeight * scale);
    const int radius = qRound(kBaseRadius * scale);

    QRect rect = option.rect.adjusted(outerMarginX, outerMarginY, -outerMarginX, -outerMarginY);

    // 背景色
    QColor bgColor = Qt::white;
    QColor borderColor = QColor(220, 220, 220);
    int remaining = index.data(LabwareRemainingRole).toInt();
    int limit = index.data(LabwareLimitRole).toInt();

    if (limit > 0 && remaining <= 0) {
        bgColor = QColor(245, 245, 245);
        borderColor = QColor(225, 225, 225);
    }

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
    painter->drawRoundedRect(rect, radius, radius);

    // 图标区域
    QRect iconRect(rect.left() + innerPadding,
        rect.top() + innerPadding,
        iconSize,
        iconSize);

    QVariant iconData = index.data(Qt::DecorationRole);
    if (iconData.canConvert<QIcon>()) {
        QIcon icon = qvariant_cast<QIcon>(iconData);
        icon.paint(painter, iconRect);
    }

    // 文字区域
    const int textLeft = iconRect.right() + spacing;
    const int textWidth = rect.right() - innerPadding - textLeft;

    QRect titleRect(textLeft,
        rect.top() + innerPadding,
        textWidth,
        titleHeight);

    QRect subRect(textLeft,
        titleRect.bottom(),
        textWidth,
        subHeight);

    QString title = index.data(Qt::DisplayRole).toString();

    QFont titleFont = QApplication::font();
    titleFont.setPointSize(qMax(8, qRound(kBaseTitlePointSize * scale)));
    titleFont.setBold(true);

    QFont subFont = QApplication::font();
    subFont.setPointSize(qMax(7, qRound(kBaseSubPointSize * scale)));

    painter->setFont(titleFont);
    painter->setPen(QColor(40, 40, 40));
    painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, title);

    painter->setFont(subFont);
    if (limit > 0 && remaining <= 0)
        painter->setPen(QColor(170, 170, 170));
    else
        painter->setPen(QColor(130, 130, 130));

    QString subText;
    if (limit > 0)
        subText = QStringLiteral("剩余 %1 / %2").arg(remaining).arg(limit);
    else
        subText = QStringLiteral("数量不限");

    painter->drawText(subRect, Qt::AlignLeft | Qt::AlignVCenter, subText);

    painter->restore();
}

QSize LabwareItemDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
{
    qreal scale = qMax<qreal>(0.6, qreal(m_viewWidth) / qreal(kBaseItemWidth));
    int itemHeight = qRound(kBaseItemHeight * scale);
    return QSize(m_viewWidth, itemHeight);
}
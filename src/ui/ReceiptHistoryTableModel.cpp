#include "ui/ReceiptHistoryTableModel.h"

#include <QDateTime>
#include <QString>

#include <cstdint>
#include <cstdlib>
#include <utility>

namespace
{

QString formatMoney(pos::Money value)
{
    const bool negative = value < 0;

    const std::int64_t absoluteValue = negative ? -value : value;

    const std::int64_t rubles = absoluteValue / 100;

    const std::int64_t kopecks = absoluteValue % 100;

    return QStringLiteral("%1%2,%3 ₽")
        .arg(negative ? QStringLiteral("-") : QString())
        .arg(static_cast<qlonglong>(rubles))
        .arg(static_cast<qlonglong>(kopecks), 2, 10, QChar('0'));
}

}

namespace pos::ui
{

ReceiptHistoryTableModel::ReceiptHistoryTableModel(QObject* parent) : QAbstractTableModel(parent)
{
}

int ReceiptHistoryTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return static_cast<int>(receipts_.size());
}

int ReceiptHistoryTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return ColumnCount;
}

QVariant ReceiptHistoryTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const application::ReceiptSummary& receipt = receipts_.at(static_cast<std::size_t>(index.row()));

    if (role == Qt::TextAlignmentRole) {
        if (index.column() == DateColumn) {
            return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);
        }

        return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
    }

    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (index.column()) {
    case IdColumn:
        return static_cast<qlonglong>(receipt.id);

    case DateColumn:
        return QDateTime::fromSecsSinceEpoch(receipt.createdAtUnixSeconds).toString("dd.MM.yyyy HH:mm:ss");

    case TotalColumn:
        return formatMoney(receipt.total);

    case ReceivedColumn:
        return formatMoney(receipt.received);

    case ChangeColumn:
        return formatMoney(receipt.change);

    default:
        return {};
    }
}

QVariant ReceiptHistoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case IdColumn:
        return QStringLiteral("№");

    case DateColumn:
        return QStringLiteral("Дата");

    case TotalColumn:
        return QStringLiteral("Сумма");

    case ReceivedColumn:
        return QStringLiteral("Получено");

    case ChangeColumn:
        return QStringLiteral("Сдача");

    default:
        return {};
    }
}

void ReceiptHistoryTableModel::setReceipts(std::vector<application::ReceiptSummary> receipts)
{
    beginResetModel();

    receipts_ = std::move(receipts);

    endResetModel();
}

}
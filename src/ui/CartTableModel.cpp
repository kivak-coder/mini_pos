#include "ui/CartTableModel.h"

#include "ui/MoneyFormatter.h"

#include <QString>

CartTableModel::CartTableModel(
    const pos::Sale& sale,
    QObject* parent)
    : QAbstractTableModel(parent),
      sale_(sale)
{
}

int CartTableModel::rowCount(
    const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return static_cast<int>(sale_.items().size());
}

int CartTableModel::columnCount(
    const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return ColumnCount;
}

QVariant CartTableModel::data(
    const QModelIndex& index,
    int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    const pos::CartItem* item = itemAt(index.row());

    if (item == nullptr)
    {
        return {};
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case NameColumn:
            return QString::fromUtf8(
                item->product().name().c_str());

        case PriceColumn:
            return pos::ui::formatMoney(
                item->product().price());

        case QuantityColumn:
            return item->quantity();

        case TotalColumn:
            return pos::ui::formatMoney(
                item->lineTotal());

        default:
            return {};
        }
    }

    if (role == Qt::TextAlignmentRole &&
        index.column() != NameColumn)
    {
        return static_cast<int>(
            Qt::AlignRight | Qt::AlignVCenter);
    }

    return {};
}

QVariant CartTableModel::headerData(
    int section,
    Qt::Orientation orientation,
    int role) const
{
    if (orientation != Qt::Horizontal ||
        role != Qt::DisplayRole)
    {
        return {};
    }

    switch (section)
    {
    case NameColumn:
        return tr("Товар");

    case PriceColumn:
        return tr("Цена");

    case QuantityColumn:
        return tr("Количество");

    case TotalColumn:
        return tr("Сумма");

    default:
        return {};
    }
}

pos::ProductId CartTableModel::productIdAt(
    int row) const noexcept
{
    const pos::CartItem* item = itemAt(row);

    if (item == nullptr)
    {
        return 0;
    }

    return item->product().id();
}

int CartTableModel::quantityAt(int row) const noexcept
{
    const pos::CartItem* item = itemAt(row);

    if (item == nullptr)
    {
        return 0;
    }

    return item->quantity();
}

void CartTableModel::refresh()
{
    beginResetModel();
    endResetModel();
}

const pos::CartItem* CartTableModel::itemAt(
    int row) const noexcept
{
    if (row < 0 ||
        row >= static_cast<int>(sale_.items().size()))
    {
        return nullptr;
    }

    return &sale_.items().at(
        static_cast<std::size_t>(row));
}
#include "ui/ProductTableModel.h"

#include "ui/MoneyFormatter.h"

#include <QString>

ProductTableModel::ProductTableModel(
    const pos::ProductCatalog& catalog,
    QObject* parent)
    : QAbstractTableModel(parent),
      catalog_(catalog)
{
}

int ProductTableModel::rowCount(
    const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return static_cast<int>(catalog_.size());
}

int ProductTableModel::columnCount(
    const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return ColumnCount;
}

QVariant ProductTableModel::data(
    const QModelIndex& index,
    int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    const pos::Product* product = productAt(index.row());

    if (product == nullptr)
    {
        return {};
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case NameColumn:
            return QString::fromUtf8(product->name().c_str());

        case PriceColumn:
            return pos::ui::formatMoney(product->price());

        default:
            return {};
        }
    }

    if (role == Qt::TextAlignmentRole &&
        index.column() == PriceColumn)
    {
        return static_cast<int>(
            Qt::AlignRight | Qt::AlignVCenter);
    }

    return {};
}

QVariant ProductTableModel::headerData(
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

    default:
        return {};
    }
}

const pos::Product* ProductTableModel::productAt(
    int row) const noexcept
{
    if (row < 0 ||
        row >= static_cast<int>(catalog_.size()))
    {
        return nullptr;
    }

    return &catalog_.at(static_cast<std::size_t>(row));
}
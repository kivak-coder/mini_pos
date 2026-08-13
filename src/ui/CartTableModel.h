#pragma once

#include "domain/Sale.h"

#include <QAbstractTableModel>

class CartTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CartTableModel(
        const pos::Sale& sale,
        QObject* parent = nullptr);

    int rowCount(
        const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(
        const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(
        const QModelIndex& index,
        int role = Qt::DisplayRole) const override;

    QVariant headerData(
        int section,
        Qt::Orientation orientation,
        int role = Qt::DisplayRole) const override;

    pos::ProductId productIdAt(int row) const noexcept;
    int quantityAt(int row) const noexcept;

    void refresh();

private:
    enum Column
    {
        NameColumn,
        PriceColumn,
        QuantityColumn,
        TotalColumn,
        ColumnCount
    };

    const pos::CartItem* itemAt(int row) const noexcept;

    const pos::Sale& sale_;
};
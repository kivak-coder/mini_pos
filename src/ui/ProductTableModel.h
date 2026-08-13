 #pragma once

#include "domain/ProductCatalog.h"

#include <QAbstractTableModel>

class ProductTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ProductTableModel(
        const pos::ProductCatalog& catalog,
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

    const pos::Product* productAt(int row) const noexcept;

private:
    enum Column
    {
        NameColumn,
        PriceColumn,
        ColumnCount
    };

    const pos::ProductCatalog& catalog_;
};
#pragma once

#include "application/ReceiptReader.h"

#include <QAbstractTableModel>

#include <vector>

namespace pos::ui
{

class ReceiptHistoryTableModel final
    : public QAbstractTableModel
{
public:
    explicit ReceiptHistoryTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setReceipts(std::vector<application::ReceiptSummary> receipts);

private:
    enum Column
    {
        IdColumn,
        DateColumn,
        TotalColumn,
        ReceivedColumn,
        ChangeColumn,
        ColumnCount
    };

    std::vector<application::ReceiptSummary> receipts_;
};

}
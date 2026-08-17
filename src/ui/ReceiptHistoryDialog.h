#pragma once

#include "application/ReceiptReader.h"
#include "ui/ReceiptHistoryTableModel.h"

#include <QDialog>

class QTableView;

namespace pos::ui
{

class ReceiptHistoryDialog final
    : public QDialog
{
public:
    explicit ReceiptHistoryDialog(const application::ReceiptReader& reader, QWidget* parent = nullptr);

private:
    void reload();

    const application::ReceiptReader& reader_;

    ReceiptHistoryTableModel* model_;
    QTableView* table_;
};

}
#include "ui/ReceiptHistoryDialog.h"

#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

#include <exception>

namespace pos::ui
{

ReceiptHistoryDialog::ReceiptHistoryDialog(const application::ReceiptReader& reader, QWidget* parent): QDialog(parent), reader_(reader),
 model_(new ReceiptHistoryTableModel(this)), table_(new QTableView(this))
{
    setWindowTitle(tr("История чеков"));
    resize(850, 450);

    table_->setModel(model_);

    table_->setSelectionBehavior(QAbstractItemView::SelectRows);

    table_->setSelectionMode(QAbstractItemView::SingleSelection);

    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    table_->setAlternatingRowColors(true);

    table_->verticalHeader()->setVisible(false);

    table_->horizontalHeader()->setStretchLastSection(true);

    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    auto* reloadButton = new QPushButton(tr("Обновить"), this);

    auto* closeButton = new QPushButton(tr("Закрыть"), this);

    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(reloadButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(table_);
    mainLayout->addLayout(buttonLayout);

    connect(reloadButton, &QPushButton::clicked, this, &ReceiptHistoryDialog::reload);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    reload();
}

void ReceiptHistoryDialog::reload()
{
    try {
        model_->setReceipts(reader_.findAll());
    }
    catch (const std::exception& error) {
        QMessageBox::critical(this, tr("Ошибка базы данных"), QString::fromUtf8(error.what()));
    }
}

}
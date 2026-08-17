#pragma once

#include "application/ReceiptRepository.h"
#include "application/ReceiptReader.h"

#include <QString>

namespace pos::infrastructure
{

class SqliteReceiptRepository final
    : public application::ReceiptRepository, 
      public application::ReceiptReader
{
public:
    explicit SqliteReceiptRepository(
        QString connectionName);

    ReceiptId save(
        const Receipt& receipt) override;

    std::vector<application::ReceiptSummary>
    findAll() const override;

private:
    QString connectionName_;
};

}
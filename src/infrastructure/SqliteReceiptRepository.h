#pragma once

#include "application/ReceiptRepository.h"

#include <QString>

namespace pos::infrastructure
{

class SqliteReceiptRepository final
    : public application::ReceiptRepository
{
public:
    explicit SqliteReceiptRepository(
        QString connectionName);

    ReceiptId save(
        const Receipt& receipt) override;

private:
    QString connectionName_;
};

}
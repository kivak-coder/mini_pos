#pragma once

#include "domain/Money.h"
#include "domain/Receipt.h"

#include <cstdint>
#include <vector>

namespace pos::application
{

struct ReceiptSummary final
{
    ReceiptId id;

    std::int64_t createdAtUnixSeconds;

    Money total;
    Money received;
    Money change;
};

class ReceiptReader
{
public:
    virtual ~ReceiptReader() = default;

    virtual std::vector<ReceiptSummary>
    findAll() const = 0;
};

}
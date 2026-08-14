#pragma once

#include "domain/Receipt.h"

namespace pos::application
{

class ReceiptRepository
{
public:
    virtual ~ReceiptRepository() = default;

    virtual ReceiptId save(
        const Receipt& receipt) = 0;
};

}
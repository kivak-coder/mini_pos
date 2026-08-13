#pragma once

#include "domain/Money.h"
#include "domain/Sale.h"

namespace pos::application
{

enum class PaymentStatus
{
    Success,
    EmptySale,
    NonPositiveAmount,
    InsufficientFunds
};

struct CashPaymentResult final
{
    PaymentStatus status = PaymentStatus::EmptySale;

    Money total = 0;
    Money received = 0;
    Money change = 0;

    bool successful() const noexcept
    {
        return status == PaymentStatus::Success;
    }
};

class CashPaymentService final
{
public:
    CashPaymentResult evaluate(
        const Sale& sale,
        Money received) const noexcept;
};

}
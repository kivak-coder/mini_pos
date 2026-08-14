#pragma once

#include "application/CashPaymentService.h"
#include "application/ReceiptRepository.h"
#include "domain/Sale.h"

namespace pos::application
{

struct CheckoutResult final
{
    CashPaymentResult payment;
    ReceiptId receiptId = 0;

    bool successful() const noexcept
    {
        return payment.successful() &&
               receiptId > 0;
    }
};

class CheckoutService final
{
public:
    CheckoutService(
        const CashPaymentService& paymentService,
        ReceiptRepository& receiptRepository);

    CheckoutResult complete(
        Sale& sale,
        Money received);

private:
    const CashPaymentService& paymentService_;
    ReceiptRepository& receiptRepository_;
};

}
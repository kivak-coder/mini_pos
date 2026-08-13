#include "application/CashPaymentService.h"

namespace pos::application 
{

CashPaymentResult CashPaymentService::evaluate(const Sale& sale, Money received) const noexcept {
    CashPaymentResult result;
    result.received = received;
    result.total = sale.total();

    if (sale.empty()) {
        result.status = PaymentStatus::EmptySale;
        return result;
    }
    if (received <= 0) {
        result.status = PaymentStatus::NonPositiveAmount;
        return result;
    }

    if (received < result.total) {
        result.status = PaymentStatus::InsufficientFunds;
        return result;
    }
    result.change = result.total - result.received;
    result.status = PaymentStatus::Success;
    return result;
}

}
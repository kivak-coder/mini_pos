#include "application/CheckoutService.h"

#include <chrono>
#include <vector>

namespace pos::application
{

CheckoutService::CheckoutService(
    const CashPaymentService& paymentService,
    ReceiptRepository& receiptRepository)
    : paymentService_(paymentService),
      receiptRepository_(receiptRepository)
{
}

CheckoutResult CheckoutService::complete(
    Sale& sale,
    Money received)
{
    CheckoutResult result;

    result.payment =
        paymentService_.evaluate(sale, received);

    if (!result.payment.successful())
    {
        return result;
    }

    std::vector<ReceiptItem> receiptItems;
    receiptItems.reserve(sale.items().size());

    for (const CartItem& item : sale.items())
    {
        receiptItems.emplace_back(
            item.product().id(),
            item.product().name(),
            item.product().price(),
            item.quantity());
    }

    const auto now =
        std::chrono::system_clock::now();

    const auto unixSeconds =
        std::chrono::duration_cast<
            std::chrono::seconds>(
                now.time_since_epoch())
            .count();

    const Receipt receipt(
        unixSeconds,
        std::move(receiptItems),
        received);

    result.receiptId =
        receiptRepository_.save(receipt);

    // Выполняется только после успешного save().
    sale.clear();

    return result;
}

}
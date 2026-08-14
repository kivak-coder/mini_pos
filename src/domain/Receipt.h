#pragma once

#include "domain/Money.h"
#include "domain/Product.h"

#include <cstdint>
#include <string>
#include <vector>

namespace pos
{

using ReceiptId = std::int64_t;

class ReceiptItem final
{
public:
    ReceiptItem(
        ProductId productId,
        std::string productName,
        Money unitPrice,
        int quantity);

    ProductId productId() const noexcept;
    const std::string& productName() const noexcept;
    Money unitPrice() const noexcept;
    int quantity() const noexcept;
    Money lineTotal() const noexcept;

private:
    ProductId productId_;
    std::string productName_;
    Money unitPrice_;
    int quantity_;
};

class Receipt final
{
public:
    Receipt(
        std::int64_t createdAtUnixSeconds,
        std::vector<ReceiptItem> items,
        Money received);

    std::int64_t createdAtUnixSeconds() const noexcept;

    const std::vector<ReceiptItem>&
    items() const noexcept;

    Money total() const noexcept;
    Money received() const noexcept;
    Money change() const noexcept;

private:
    std::int64_t createdAtUnixSeconds_;
    std::vector<ReceiptItem> items_;

    Money total_;
    Money received_;
    Money change_;
};

}
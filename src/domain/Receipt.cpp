#include "domain/Receipt.h"

#include <stdexcept>
#include <utility>

namespace pos
{

ReceiptItem::ReceiptItem(
    ProductId productId,
    std::string productName,
    Money unitPrice,
    int quantity)
    : productId_(productId),
      productName_(std::move(productName)),
      unitPrice_(unitPrice),
      quantity_(quantity)
{
    if (productId_ <= 0)
    {
        throw std::invalid_argument(
            "Receipt product id must be positive");
    }

    if (productName_.empty())
    {
        throw std::invalid_argument(
            "Receipt product name must not be empty");
    }

    if (unitPrice_ <= 0)
    {
        throw std::invalid_argument(
            "Receipt unit price must be positive");
    }

    if (quantity_ <= 0)
    {
        throw std::invalid_argument(
            "Receipt quantity must be positive");
    }
}

ProductId ReceiptItem::productId() const noexcept
{
    return productId_;
}

const std::string&
ReceiptItem::productName() const noexcept
{
    return productName_;
}

Money ReceiptItem::unitPrice() const noexcept
{
    return unitPrice_;
}

int ReceiptItem::quantity() const noexcept
{
    return quantity_;
}

Money ReceiptItem::lineTotal() const noexcept
{
    return unitPrice_ * quantity_;
}

Receipt::Receipt(
    std::int64_t createdAtUnixSeconds,
    std::vector<ReceiptItem> items,
    Money received)
    : createdAtUnixSeconds_(createdAtUnixSeconds),
      items_(std::move(items)),
      total_(0),
      received_(received),
      change_(0)
{
    if (createdAtUnixSeconds_ <= 0)
    {
        throw std::invalid_argument(
            "Receipt creation time must be positive");
    }

    if (items_.empty())
    {
        throw std::invalid_argument(
            "Receipt must contain at least one item");
    }

    for (const ReceiptItem& item : items_)
    {
        total_ += item.lineTotal();
    }

    if (received_ < total_)
    {
        throw std::invalid_argument(
            "Received amount is insufficient");
    }

    change_ = received_ - total_;
}

std::int64_t
Receipt::createdAtUnixSeconds() const noexcept
{
    return createdAtUnixSeconds_;
}

const std::vector<ReceiptItem>&
Receipt::items() const noexcept
{
    return items_;
}

Money Receipt::total() const noexcept
{
    return total_;
}

Money Receipt::received() const noexcept
{
    return received_;
}

Money Receipt::change() const noexcept
{
    return change_;
}

}
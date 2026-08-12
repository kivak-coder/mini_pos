#pragma once

#include "domain/CartItem.h"

#include <vector>

namespace pos
{

class Sale final
{
public:
    void addProduct(const Product& product, int quantity = 1);
    bool removeProduct(ProductId productId);
    bool setQuantity(ProductId productId, int quantity);

    bool contains(ProductId productId) const noexcept;
    bool empty() const noexcept;
    Money total() const noexcept;

    const std::vector<CartItem>& items() const noexcept;

    void clear() noexcept;

private:
    std::vector<CartItem>::iterator findItem(ProductId productId);
    std::vector<CartItem>::const_iterator findItem(ProductId productId) const;

    std::vector<CartItem> items_;
};

}
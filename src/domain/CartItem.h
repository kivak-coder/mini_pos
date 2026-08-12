#pragma once

#include "domain/Product.h"

namespace pos
{

class CartItem final
{
public:
    explicit CartItem(Product product, int quantity = 1);

    const Product& product() const noexcept;
    int quantity() const noexcept;
    Money lineTotal() const noexcept;

    void setQuantity(int quantity);
    void increaseQuantity(int amount = 1);

private:
    Product product_;
    int quantity_;
};

}
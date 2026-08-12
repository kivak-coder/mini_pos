#include "domain/CartItem.h"
#include <stdexcept>

namespace pos {

CartItem::CartItem(Product product, int quantity) : product_(std::move(product)), quantity_(quantity) 
{
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be positive!");
    }
}

const Product& CartItem::product() const noexcept {
    return product_;
}

int CartItem::quantity() const noexcept {
    return quantity_;
}

Money CartItem::lineTotal() const noexcept {
    return quantity_ * product_.price();
}


void CartItem::setQuantity(int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be positive!");
    }
    this->quantity_ = quantity;
}

void CartItem::increaseQuantity(int amount) {
    if (amount <= 0) {
        throw std::invalid_argument("Increase must be positive!");
    }
    this->quantity_ += amount;
}

}
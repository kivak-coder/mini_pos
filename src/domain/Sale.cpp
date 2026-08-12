#include "domain/Sale.h"
#include <stdexcept>
#include <algorithm>

namespace pos {

void Sale::addProduct(const Product& product, int quantity) 
{
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be positive");
    }
    auto item = this->findItem(product.id());
    if (item == this->items().end()) {
        this->items_.emplace_back(product, quantity);
        return;
    } 
    item->increaseQuantity(quantity);
}

bool Sale::removeProduct(ProductId productId) {
    auto item = this->findItem(productId);
    if (item == this->items().end()) {
        return false;
    }
    items_.erase(item);
    return true;
}

bool Sale::setQuantity(ProductId productId, int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be positive");
    }
    auto item = this->findItem(productId);
    if (item == this->items().end()) {
        return false;
    }
    item->setQuantity(quantity);
    return true;

}

bool Sale::contains(ProductId productId) const noexcept {
    auto item = this->findItem(productId);
    return item != this->items().end();

}

bool Sale::empty() const noexcept {
    return items_.empty();
}

Money Sale::total() const noexcept {
    Money total = 0;
    for (const CartItem& item : this->items_) {
        total += item.lineTotal();
    }
    return total;
}

const std::vector<CartItem>& Sale::items() const noexcept {
    return this->items_;
}

void Sale::clear() noexcept {
    this->items_.clear();
}

std::vector<CartItem>::iterator Sale::findItem(ProductId productId) {
    return std::find_if(items_.begin(), items_.end(),  
    [productId](const CartItem& item)
        {
            return item.product().id() == productId;
        }
    );
}

std::vector<CartItem>::const_iterator Sale::findItem(ProductId productId) const {
    return std::find_if(items_.cbegin(), items_.cend(),  
    [productId](const CartItem& item)
        {
            return item.product().id() == productId;
        }
    );
}

}
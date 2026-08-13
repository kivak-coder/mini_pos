#include "domain\ProductCatalog.h"
#include <algorithm>
#include <stdexcept>

namespace pos {

ProductCatalog::ProductCatalog(std::vector<Product> products) : products_(std::move(products)) {
    for (std::size_t i = 0; i < products_.size(); ++i)
    {
        for (std::size_t j = i + 1; j < products_.size(); ++j)
        {
            if (products_[i].id() == products_[j].id())
            {
                throw std::invalid_argument(
                    "Product catalog contains duplicate ids");
            }
        }
    }
}

std::size_t ProductCatalog::size() const noexcept {
    return this->products_.size();
}

bool ProductCatalog::empty() const noexcept {
    return this->products_.empty();
}

const Product& ProductCatalog::at(std::size_t index) const {
    return this->products_.at(index);
}

const std::vector<Product>& ProductCatalog::products() const noexcept {
    return this->products_;
}

const Product* ProductCatalog::findById(ProductId productId) const noexcept {
    const auto product = std::find_if(
        products_.cbegin(),
        products_.cend(),
        [productId](const Product& candidate)
        {
            return candidate.id() == productId;
        });
        if (product == products_.end()) {
            return nullptr;
        }
        return &*product;
}

}
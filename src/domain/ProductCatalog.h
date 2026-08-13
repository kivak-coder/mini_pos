#pragma once

#include "domain/Product.h"

#include <cstddef>
#include <vector>

namespace pos
{

class ProductCatalog final
{
public:
    explicit ProductCatalog(std::vector<Product> products);

    std::size_t size() const noexcept;
    bool empty() const noexcept;

    const Product& at(std::size_t index) const;
    const std::vector<Product>& products() const noexcept;
    const Product* findById(ProductId productId) const noexcept;

private:
    std::vector<Product> products_;
};

}
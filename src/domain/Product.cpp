#include "domain/Product.h"

#include <stdexcept>
#include <utility>

namespace pos
{

Product::Product(ProductId id, std::string name, Money price)
    : id_(id),
      name_(std::move(name)),
      price_(price)
{
    if (id_ <= 0)
    {
        throw std::invalid_argument("Product id must be positive");
    }

    if (name_.empty())
    {
        throw std::invalid_argument("Product name must not be empty");
    }

    if (price_ <= 0)
    {
        throw std::invalid_argument("Product price must be positive");
    }
}

ProductId Product::id() const noexcept
{
    return id_;
}

const std::string& Product::name() const noexcept
{
    return name_;
}

Money Product::price() const noexcept
{
    return price_;
}

}
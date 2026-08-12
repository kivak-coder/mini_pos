#pragma once

#include "domain/Money.h"

#include <cstdint>
#include <string>

namespace pos
{

using ProductId = std::int64_t;

class Product final
{
public:
    Product(ProductId id, std::string name, Money price);

    ProductId id() const noexcept;
    const std::string& name() const noexcept;
    Money price() const noexcept;

private:
    ProductId id_;
    std::string name_;
    Money price_;
};

}
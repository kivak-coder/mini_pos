#include "infrastructure/ProductCatalogDemo.h"

#include <vector>

namespace pos::infrastructure
{

ProductCatalog makeDemoProductCatalog()
{
    std::vector<Product> products {
        {1, u8"Молоко", 8990},
        {2, u8"Хлеб", 5990},
        {3, u8"Яблоки", 12990},
        {4, u8"Сыр", 24990},
        {5, u8"Кофе", 39990},
        {6, u8"Чай", 18990},
        {7, u8"Шоколад", 11990},
        {8, u8"Вода", 4990}
    };

    return ProductCatalog{std::move(products)};
}

}
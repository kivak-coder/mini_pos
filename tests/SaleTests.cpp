#define BOOST_TEST_MODULE SaleTests

#include <boost/test/included/unit_test.hpp>

#include "domain/Product.h"
#include "domain/Sale.h"

#include <stdexcept>

namespace
{

const pos::Product apple{1, "Apple", 12990};
const pos::Product milk{2, "Milk", 8990};

}

BOOST_AUTO_TEST_CASE(new_sale_is_empty)
{
    const pos::Sale sale;

    BOOST_TEST(sale.empty());
    BOOST_TEST(sale.total() == 0);
}

BOOST_AUTO_TEST_CASE(product_can_be_added)
{
    pos::Sale sale;

    sale.addProduct(apple);

    BOOST_TEST(!sale.empty());
    BOOST_TEST(sale.items().size() == 1U);
    BOOST_TEST(sale.items().front().quantity() == 1);
    BOOST_TEST(sale.total() == 12990);
}

BOOST_AUTO_TEST_CASE(adding_same_product_increases_quantity)
{
    pos::Sale sale;

    sale.addProduct(apple);
    sale.addProduct(apple, 2);

    BOOST_TEST(sale.items().size() == 1U);
    BOOST_TEST(sale.items().front().quantity() == 3);
    BOOST_TEST(sale.total() == 38970);
}

BOOST_AUTO_TEST_CASE(total_contains_all_positions)
{
    pos::Sale sale;

    sale.addProduct(apple, 2);
    sale.addProduct(milk, 3);

    BOOST_TEST(sale.total() == 52950);
}

BOOST_AUTO_TEST_CASE(quantity_can_be_changed)
{
    pos::Sale sale;
    sale.addProduct(milk);

    const bool changed = sale.setQuantity(milk.id(), 4);

    BOOST_TEST(changed);
    BOOST_TEST(sale.total() == 35960);
}

BOOST_AUTO_TEST_CASE(product_can_be_removed)
{
    pos::Sale sale;
    sale.addProduct(apple);
    sale.addProduct(milk);

    const bool removed = sale.removeProduct(apple.id());

    BOOST_TEST(removed);
    BOOST_TEST(!sale.contains(apple.id()));
    BOOST_TEST(sale.contains(milk.id()));
    BOOST_TEST(sale.total() == 8990);
}

BOOST_AUTO_TEST_CASE(quantity_must_be_positive)
{
    pos::Sale sale;

    BOOST_CHECK_THROW(
        sale.addProduct(apple, 0),
        std::invalid_argument);

    sale.addProduct(apple);

    BOOST_CHECK_THROW(
        sale.setQuantity(apple.id(), -1),
        std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(sale_can_be_cleared)
{
    pos::Sale sale;
    sale.addProduct(apple);
    sale.addProduct(milk);

    sale.clear();

    BOOST_TEST(sale.empty());
    BOOST_TEST(sale.total() == 0);
}
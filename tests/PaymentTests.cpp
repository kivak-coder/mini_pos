#define BOOST_TEST_MODULE CashPaymentTests

#include <boost/test/included/unit_test.hpp>

#include "application/CashPaymentService.h"
#include "domain/Product.h"
#include "domain/Sale.h"

namespace
{

const pos::Product apple{1, "Apple", 12990};
const pos::Product milk{2, "Milk", 8990};

int statusCode(pos::application::PaymentStatus status)
{
    return static_cast<int>(status);
}

}

BOOST_AUTO_TEST_SUITE(cash_payment)

BOOST_AUTO_TEST_CASE(empty_sale_is_rejected)
{
    const pos::Sale sale;
    const pos::application::CashPaymentService service;

    const auto result = service.evaluate(sale, 10000);

    BOOST_TEST(!result.successful());

    BOOST_TEST(
        statusCode(result.status) ==
        statusCode(pos::application::PaymentStatus::EmptySale));
}

BOOST_AUTO_TEST_CASE(non_positive_amount_is_rejected)
{
    pos::Sale sale;
    sale.addProduct(apple);

    const pos::application::CashPaymentService service;

    const auto zeroResult = service.evaluate(sale, 0);
    const auto negativeResult = service.evaluate(sale, -100);

    BOOST_TEST(
        statusCode(zeroResult.status) ==
        statusCode(
            pos::application::PaymentStatus::NonPositiveAmount));

    BOOST_TEST(
        statusCode(negativeResult.status) ==
        statusCode(
            pos::application::PaymentStatus::NonPositiveAmount));
}

BOOST_AUTO_TEST_CASE(insufficient_amount_is_rejected)
{
    pos::Sale sale;
    sale.addProduct(apple);

    const pos::application::CashPaymentService service;

    const auto result = service.evaluate(sale, 10000);

    BOOST_TEST(!result.successful());

    BOOST_TEST(
        statusCode(result.status) ==
        statusCode(
            pos::application::PaymentStatus::InsufficientFunds));

    BOOST_TEST(result.total == 12990);
    BOOST_TEST(result.change == 0);
}

BOOST_AUTO_TEST_CASE(exact_amount_produces_zero_change)
{
    pos::Sale sale;
    sale.addProduct(apple);

    const pos::application::CashPaymentService service;

    const auto result = service.evaluate(sale, 12990);

    BOOST_TEST(result.successful());
    BOOST_TEST(result.total == 12990);
    BOOST_TEST(result.received == 12990);
    BOOST_TEST(result.change == 0);
}

BOOST_AUTO_TEST_CASE(excess_amount_produces_change)
{
    pos::Sale sale;
    sale.addProduct(apple);
    sale.addProduct(milk);

    const pos::application::CashPaymentService service;

    const auto result = service.evaluate(sale, 25000);

    BOOST_TEST(result.successful());
    BOOST_TEST(result.total == 21980);
    BOOST_TEST(result.change == 3020);
}

BOOST_AUTO_TEST_CASE(evaluation_does_not_modify_sale)
{
    pos::Sale sale;
    sale.addProduct(apple, 2);

    const pos::application::CashPaymentService service;

    const auto result = service.evaluate(sale, 30000);

    BOOST_TEST(result.successful());
    BOOST_TEST(!sale.empty());
    BOOST_TEST(sale.items().size() == 1U);
    BOOST_TEST(sale.items().front().quantity() == 2);
    BOOST_TEST(sale.total() == 25980);
}

BOOST_AUTO_TEST_SUITE_END()
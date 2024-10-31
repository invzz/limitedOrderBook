#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>
#include "book.h" // Include your order book header

static void test_add_order(void **state)
{
    OrderBook *order_book = create_hybrid_order_book(10);
    Order order = {1, 100, 1, NULL};

    order_book->add_order(order_book, order);

    // Check if the order was added (implement your own check logic)
    assert_non_null(order_book->price_levels); // Example check
    // Add further assertions as necessary

    destroy_order_book(order_book);
}

static void test_remove_order(void **state)
{
    OrderBook *order_book = create_hybrid_order_book(10);
    Order order = {2, 200, 2, NULL};

    order_book->add_order(order_book, order);
    order_book->remove_order(order_book, 2); // Remove the order

    // Check if the order was removed (implement your own check logic)

    destroy_order_book(order_book);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add_order),
        cmocka_unit_test(test_remove_order),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

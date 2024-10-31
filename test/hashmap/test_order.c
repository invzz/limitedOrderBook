#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>
#include "order.h" // Include your order header

// You can define unit tests for Order operations here

static void test_order_creation(void **state)
{
    Order *order = malloc(sizeof(Order));
    order->id = 1;
    order->quantity = 100;
    order->timestamp = 1;
    order->next = NULL;

    assert_int_equal(order->id, 1);
    assert_int_equal(order->quantity, 100);

    free(order);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_order_creation),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

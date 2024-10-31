#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>
#include "structs.h" // Include your structs header

// You can define unit tests for Structs operations here

static void test_price_level_creation(void **state)
{
    PriceLevel *price_level = malloc(sizeof(PriceLevel));
    price_level->price = 100.0;
    price_level->orders = NULL;
    price_level->next = NULL;

    assert_float_equal(price_level->price, 100.0);

    free(price_level);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_price_level_creation),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

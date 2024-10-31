#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "book.h"
#include "order.h"

#define NUM_OPERATIONS 10000 // Number of operations to perform in the test

// Function to perform stress testing on the order book
void stress_test(OrderBook *order_book)
{
    // Variables for timing
    clock_t start, end;
    double cpu_time_used;

    // Add operations
    start = clock();
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        Order order = {i, rand() % 100 + 1, i, NULL}; // Random quantity and timestamp
        order_book->add_order(order_book, order);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Added %d orders in %.2f seconds. Operations per second: %.2f\n",
           NUM_OPERATIONS, cpu_time_used, NUM_OPERATIONS / cpu_time_used);

    // Matching operations
    start = clock();
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        Order buy_order = {NUM_OPERATIONS + i, rand() % 100 + 1, i, NULL}; // Buy order
        Order sell_order = {i, rand() % 100 + 1, i, NULL};                 // Sell order
        order_book->add_order(order_book, sell_order);                     // Add sell order
        order_book->add_order(order_book, buy_order);                      // Add buy order
        // Add simple matching logic here (pseudo-code)
        // match_orders(order_book, buy_order, sell_order); // Match buy/sell orders
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Matched %d orders in %.2f seconds. Operations per second: %.2f\n",
           NUM_OPERATIONS, cpu_time_used, NUM_OPERATIONS / cpu_time_used);

    // Remove operations
    start = clock();
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        order_book->remove_order(order_book, i); // Removing orders by ID
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Removed %d orders in %.2f seconds. Operations per second: %.2f\n",
           NUM_OPERATIONS, cpu_time_used, NUM_OPERATIONS / cpu_time_used);
}

int main()
{
    // Seed the random number generator
    srand(time(NULL));

    // Create hybrid order book
    int capacity = 100; // Define the capacity of the hash map
    OrderBook *order_book = create_hybrid_order_book(capacity);

    // Run the stress test
    stress_test(order_book);

    // Clean up
    destroy_order_book(order_book);

    return 0;
}

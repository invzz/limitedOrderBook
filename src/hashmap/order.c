#include <stdlib.h>
#include "order.h"

// Function to create a new order
Order *create_order(int id, int quantity, int timestamp)
{
    Order *new_order = (Order *)malloc(sizeof(Order));
    if (new_order)
    {
        new_order->id = id;
        new_order->quantity = quantity;
        new_order->timestamp = timestamp;
        new_order->next = NULL; // Initialize next pointer
    }
    return new_order;
}

// Function to free an order
void free_order(Order *order)
{
    if (order)
    {
        free(order);
    }
}

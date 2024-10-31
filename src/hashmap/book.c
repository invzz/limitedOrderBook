// book.c

#include <stdlib.h>
#include <stdio.h>
#include "structs.h"

// Hash function to map a price to an index
unsigned int hash(double price, int capacity)
{
    return (unsigned int)(price) % capacity; // Simple hash function
}

// Function to add an order to the hybrid implementation
void hybrid_add_order(OrderBook *self, Order order)
{
    unsigned int index = hash((double)order.id, self->capacity); // Use order ID as a price for simplicity

    // Create a new order and add it to the price level
    Order *new_order = malloc(sizeof(Order));
    if (new_order == NULL)
    {
        perror("Failed to allocate memory for new order");
        return;
    }
    *new_order = order;
    new_order->next = NULL;

    // If there is no price level at this index, create one
    if (self->price_levels[index] == NULL)
    {
        self->price_levels[index] = malloc(sizeof(PriceLevel));
        if (self->price_levels[index] == NULL)
        {
            perror("Failed to allocate memory for new price level");
            free(new_order);
            return;
        }
        self->price_levels[index]->price = (double)order.id; // Assign the price from the order ID for simplicity
        self->price_levels[index]->orders = new_order;
        self->price_levels[index]->next = NULL;
    }
    else
    {
        // Add the order to the existing price level's order list
        PriceLevel *price_level = self->price_levels[index];
        new_order->next = price_level->orders; // Insert at the front
        price_level->orders = new_order;
    }
}

// Function to remove an order from the hybrid implementation
void hybrid_remove_order(OrderBook *self, int order_id)
{
    unsigned int index = hash((double)order_id, self->capacity);

    PriceLevel *price_level = self->price_levels[index];
    if (price_level == NULL)
        return; // No price level at this index

    Order *current = price_level->orders;
    Order *previous = NULL;

    // Search for the order to remove
    while (current != NULL)
    {
        if (current->id == order_id)
        {
            // Found the order to remove
            if (previous == NULL)
            {
                // Removing the first order
                price_level->orders = current->next;
            }
            else
            {
                // Removing a non-first order
                previous->next = current->next;
            }
            free(current); // Free the memory of the removed order
            return;
        }
        previous = current;
        current = current->next;
    }
}

// Function to create a hybrid order book
OrderBook *create_hybrid_order_book(int capacity)
{
    OrderBook *order_book = (OrderBook *)malloc(sizeof(OrderBook));
    if (order_book)
    {
        order_book->capacity = capacity;
        order_book->price_levels = (PriceLevel **)calloc(capacity, sizeof(PriceLevel *)); // Allocate space for hash map
        order_book->add_order = hybrid_add_order;                                         // Set function pointer for adding orders
        order_book->remove_order = hybrid_remove_order;                                   // Set function pointer for removing orders
    }
    return order_book;
}

// Function to destroy the hybrid order book
void destroy_order_book(OrderBook *order_book)
{
    for (int i = 0; i < order_book->capacity; i++)
    {
        PriceLevel *price_level = order_book->price_levels[i];
        while (price_level != NULL)
        {
            Order *order = price_level->orders;
            while (order != NULL)
            {
                Order *temp = order;
                order = order->next;
                free(temp); // Free each order
            }
            PriceLevel *temp_level = price_level;
            price_level = price_level->next;
            free(temp_level); // Free each price level
        }
    }
    free(order_book->price_levels); // Free the hash map array
    free(order_book);               // Finally, free the order book
}

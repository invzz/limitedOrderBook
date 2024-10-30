#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H
#include <stdio.h>
#include <stdlib.h>

typedef struct Order
{
    int id;             // Unique order ID
    double price;       // Price of the order
    int quantity;       // Quantity for the order
    int timestamp;      // Timestamp to resolve order precedence
    struct Order *next; // Pointer to the znext order in the list
} Order;

typedef struct OrderBook
{
    Order *bids; // Linked list for bid orders
    Order *asks; // Linked list for ask orders
} OrderBook;

#define NUM_ASSETS 3 // Number of assets in our simulation

#endif
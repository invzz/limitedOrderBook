// structs.h

#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct Order
{
    int id;
    int quantity;
    int timestamp;
    struct Order *next;
} Order;

typedef struct PriceLevel
{
    double price;
    Order *orders;           // Linked list of orders at this price level
    struct PriceLevel *next; // Next price level in sorted order
} PriceLevel;

// Abstract OrderBook struct
typedef struct OrderBook
{
    PriceLevel **price_levels;                                  // Hash map (array of pointers to price levels)
    int capacity;                                               // Capacity of the hash map
    void (*add_order)(struct OrderBook *self, Order order);     // Pointer to add order function
    void (*remove_order)(struct OrderBook *self, int order_id); // Pointer to remove order function
} OrderBook;

// Function prototypes
OrderBook *create_hybrid_order_book(int capacity);
void destroy_order_book(OrderBook *order_book);

#endif // STRUCTS_H

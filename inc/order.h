#ifndef ORDER_H
#define ORDER_H
#include <data_structs.h>
Order *create_order(int id, double price, int quantity, int timestamp)
{
    Order *order = (Order *)malloc(sizeof(Order));
    order->id = id;
    order->price = price;
    order->quantity = quantity;
    order->timestamp = timestamp;
    order->next = NULL;
    return order;
}

void add_order(Order **head, Order *new_order, int is_bid)
{
    Order **current = head;
    while (*current && ((is_bid && (*current)->price > new_order->price) ||
                        (!is_bid && (*current)->price < new_order->price) ||
                        ((*current)->price == new_order->price && (*current)->timestamp <= new_order->timestamp)))
    {
        current = &(*current)->next;
    }
    new_order->next = *current;
    *current = new_order;
}
#endif

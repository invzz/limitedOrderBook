#ifndef BOOK_H
#define BOOK_H
#include <order.h>
#include <structs.h>
#define MAX_PRICE_LEVELS 10000
extern PriceLevel *bid_head;
extern PriceLevel *ask_head;

void add_order_to_price_level(PriceLevel **head, double price, int id, int quantity, int timestamp);
void match_orders();
void print_order_book(PriceLevel *head, int is_bid);

#endif
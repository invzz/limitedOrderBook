#ifndef BOOK_H
#define BOOK_H
#include <stdio.h>
#include <stdlib.h>
#include <order.h>
#include <data_structs.h>

void match_orders(OrderBook *book)
{
    while (book->bids && book->asks && book->bids->price >= book->asks->price)
    {
        int quantity = (book->bids->quantity < book->asks->quantity) ? book->bids->quantity : book->asks->quantity;
        printf("Trade executed: %d units at %.2f\n", quantity, book->asks->price);

        book->bids->quantity -= quantity;
        book->asks->quantity -= quantity;

        if (book->bids->quantity == 0)
        {
            Order *temp = book->bids;
            book->bids = book->bids->next;
            free(temp);
        }
        if (book->asks->quantity == 0)
        {
            Order *temp = book->asks;
            book->asks = book->asks->next;
            free(temp);
        }
    }
}


void add_order_to_book(OrderBook *book, int id, double price, int quantity, int timestamp, int is_bid)
{
    Order *order = create_order(id, price, quantity, timestamp);
    if (is_bid)
    {
        add_order(&(book->bids), order, 1);
        printf("Added bid: ID %d, Price %.2f, Quantity %d\n", id, price, quantity);
    }
    else
    {
        add_order(&(book->asks), order, 0);
        printf("Added ask: ID %d, Price %.2f, Quantity %d\n", id, price, quantity);
    }
    match_orders(book);
}
#endif
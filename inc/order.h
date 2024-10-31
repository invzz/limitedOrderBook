#ifndef ORDER_H
#define ORDER_H
#include <structs.h>

Order *create_order(int id, int quantity, int timestamp);
void free_order(Order *order);

#endif

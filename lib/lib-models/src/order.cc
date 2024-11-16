#include "order.hh"
#include <cassert>

// Update quantity by a specified delta
void Order::updateQuantity(int delta)
{
    quantity += delta;
    // Quantity should not go below zero
    if(quantity < 0)
        {
            quantity = 0; // Prevent negative quantities
        }
}
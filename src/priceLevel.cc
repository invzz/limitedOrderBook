#include "priceLevel.hh"

PriceLevel::PriceLevel(double price) : price(price) {}

void PriceLevel::addOrder(Order *order) { orders.push_back(order); }

void PriceLevel::removeFilledOrders()
{
  orders.erase(std::remove_if(orders.begin(), orders.end(), [](Order *order) { return order->getQuantity() <= 0; }), orders.end());
}

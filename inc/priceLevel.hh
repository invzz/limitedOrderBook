#ifndef PRICE_LEVEL_HH
#define PRICE_LEVEL_HH

#include "order.hh"
#include <vector>

class PriceLevel
{
  private:
  double               price;
  std::vector<Order *> orders;

  public:
  PriceLevel(double price);
  void addOrder(Order *order);
  void removeFilledOrders();

  // getter
  double               getPrice() const { return price; }
  std::vector<Order *> getOrders() const { return orders; }
  // setter
  void setPrice(double newPrice) { price = newPrice; }
  void setOrders(std::vector<Order *> newOrders) { orders = newOrders; }
  void removeOrder(Order *order)
  {
    orders.erase(std::remove(orders.begin(), orders.end(), order),
                 orders.end());
  }
};

#endif // PRICE_LEVEL_HH

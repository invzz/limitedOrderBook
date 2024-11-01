#ifndef ORDER_HH
#define ORDER_HH

#include "orderType.hh"
#include <string>

class Order
{
  private:
  int       id;
  OrderType type;
  double    price;
  int       quantity;

  public:
  // Constructor
  Order(int id, OrderType type, double price, int quantity);

  // Getters
  int       getId() const;
  OrderType getType() const;
  double    getPrice() const;
  int       getQuantity() const;

  // Setters
  void setId(int newId);
  void setType(OrderType newType);
  void setPrice(double newPrice);
  void setQuantity(int newQuantity);
  // Update quantity
  void updateQuantity(int delta);
};

#endif // ORDER_HH
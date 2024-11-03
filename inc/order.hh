#ifndef ORDER_HH
#define ORDER_HH

#include "orderType.hh"
#include <string>

class Order
{
  private:
  int       id;
  int       userId;
  int       quantity;
  double    price;
  OrderType type;

  public:
  // Constructor
  Order::Order(OrderType type, double price, int quantity, int userId) : id(0), type(type), price(price), quantity(quantity), userId(userId) {}

  // Getters
  int       getId() const { return id; };
  OrderType getType() const { return type; };
  double    getPrice() const { return price; };
  int       getQuantity() const { return quantity; };
  int       getUserId() const { return userId; }

  // Setters
  void setId(int newId) { id = newId; };
  void setUserId(int newUserId) { userId = newUserId; };
  void setType(OrderType newType) { type = newType; };
  void setPrice(double newPrice) { price = newPrice; };
  void setQuantity(int newQuantity) { quantity = newQuantity; };

  // Update quantity
  void updateQuantity(int delta);
};

#endif // ORDER_HH
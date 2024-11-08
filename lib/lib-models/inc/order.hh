#ifndef ORDER_HH
#define ORDER_HH
#include "orderType.hh"
#include <nlohmann/json.hpp>
#include <string>
#include <memory> // For std::shared_ptr

class Order
{
  private:
  OrderType   type;
  double      price;
  int         quantity;
  std::string userId;
  int         id;

  public:
  // Constructor
  Order(OrderType t, double p, int q, std::string uId) : type(t), price(p), quantity(q), userId(uId) { id = 0; }

  // Constructor
  Order(OrderType t, double p, int q, std::string uId, int i) : type(t), price(p), quantity(q), userId(uId), id(i) {}

  // Convert to JSON
  nlohmann::json toJson() const
  {
    return nlohmann::json{
      {"id",       id                                       },
      {"userId",   userId                                   },
      {"price",    price                                    },
      {"quantity", quantity                                 },
      {"type",     (type == OrderType::BUY ? "BUY" : "SELL")}
    };
  }

  // Create an Order from JSON (now returning shared_ptr)
  static std::shared_ptr<Order> fromJson(const nlohmann::json &orderData)
  {
    int         id       = orderData["id"];
    OrderType   type     = (orderData["type"] == "BUY") ? OrderType::BUY : OrderType::SELL;
    double      price    = orderData["price"];
    int         quantity = orderData["quantity"];
    std::string userId   = orderData["userId"];

    auto order = std::make_shared<Order>(type, price, quantity, userId);
    order->setId(id); // Set the ID
    return order;
  }

  // Getters
  int         getId() const { return id; }
  OrderType   getType() const { return type; }
  double      getPrice() const { return price; }
  int         getQuantity() const { return quantity; }
  std::string getUserId() const { return userId; }

  // Setters
  void setId(int newId) { id = newId; }
  void setUserId(int newUserId) { userId = newUserId; }
  void setType(OrderType newType) { type = newType; }
  void setPrice(double newPrice) { price = newPrice; }
  void setQuantity(int newQuantity) { quantity = newQuantity; }

  // Update quantity
  void updateQuantity(int delta);
};

#endif // ORDER_HH

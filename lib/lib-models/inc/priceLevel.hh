#ifndef PRICE_LEVEL_HH
#define PRICE_LEVEL_HH

#include "order.hh"
#include <vector>
#include <mutex>
#include <algorithm>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

class PriceLevel
{
  private:
  double                              price;
  std::vector<std::unique_ptr<Order>> orders;
  mutable std::mutex                  mtx; // Mutex to protect access to orders

  public:
  PriceLevel(double price) : price(price) {}

  int getSize() const
  {
    std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
    return orders.size();
  }

  public:
  nlohmann::json toJson() const
  {
    std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
    nlohmann::json              json;
    json["price"] = price;

    nlohmann::json ordersJson = nlohmann::json::array();
    for(const auto &order : orders)
      {
        if(order) ordersJson.push_back(order->toJson());
      }
    json["orders"] = ordersJson;

    return json;
  }

  static std::unique_ptr<PriceLevel> createFromJson(const nlohmann::json &priceLevelData)
  {
    double price      = priceLevelData["price"];
    auto   priceLevel = std::make_unique<PriceLevel>(price);

    // Deserialize each order within this price level
    for(const auto &orderData : priceLevelData["orders"])
      {
        auto order = Order::createFromJson(orderData);
        priceLevel->orders.push_back(std::move(order));
      }
    return priceLevel;
  }

  void addOrder(std::unique_ptr<Order> order)
  {
    std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
    orders.push_back(std::move(order));    // Use move semantics to transfer ownership
  }

  void removeOrder(Order *order)
  {
    std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
    orders.erase(std::remove_if(orders.begin(), orders.end(),
                                [order](const std::unique_ptr<Order> &ptr) {
                                  return ptr.get() == order; // Compare the raw pointer
                                }),
                 orders.end());
  }

  void removeFilledOrders()
  {
    std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
    orders.erase(std::remove_if(orders.begin(), orders.end(),
                                [](const std::unique_ptr<Order> &ptr) {
                                  return ptr->getQuantity() == 0; // Remove filled orders
                                }),
                 orders.end());
  }

  // Getter
  double getPrice() const { return price; }

  // Getter for orders (const method)
  std::vector<std::unique_ptr<Order>> &getOrders() { return orders; }

  // Setter
  void setPrice(double newPrice) { price = newPrice; }
  void setOrders(std::vector<std::unique_ptr<Order>> newOrders) { orders = std::move(newOrders); }
};

#endif // PRICE_LEVEL_HH
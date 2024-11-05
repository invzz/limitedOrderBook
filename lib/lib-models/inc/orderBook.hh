#ifndef ORDER_BOOK_HH
#define ORDER_BOOK_HH

#include "order.hh"
#include "priceLevel.hh"
#include <map>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include "metrics.hh"

class OrderBook
{
  public:
  void addOrder(std::unique_ptr<Order> new_order);
  void match(int tick, std::unordered_map<int, Metrics *> metricsMap = {});
  int  getNextOrderId() { return next_order_id++; }

  bool isEmpty() { return buy_orders.empty() && sell_orders.empty(); }

  double getBestBid()
  {
    if(buy_orders.empty()) return 0.0;
    // return the highest buy price
    return buy_orders.rbegin()->first;
  }

  nlohmann::json toJson() const
  {
    nlohmann::json json;

    {
      std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
      // Serialize buy orders
      nlohmann::json buyOrdersJson = nlohmann::json::array();
      for(const auto &[price, level] : buy_orders)
        {
          if(level) buyOrdersJson.push_back(level->toJson());
        }
      json["buy_orders"] = buyOrdersJson;

      // Serialize sell orders
      nlohmann::json sellOrdersJson = nlohmann::json::array();
      for(const auto &[price, level] : sell_orders)
        {
          if(level) sellOrdersJson.push_back(level->toJson());
        }
      json["sell_orders"] = sellOrdersJson;
    }

    return json;
  }

  void clear()
  {
    std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
    buy_orders.clear();
    sell_orders.clear();
  }

  // Getters
  std::map<double, std::unique_ptr<PriceLevel>> &getBuyOrders() { return buy_orders; }
  std::map<double, std::unique_ptr<PriceLevel>> &getSellOrders() { return sell_orders; }

  private:
  std::map<double, std::unique_ptr<PriceLevel>> buy_orders;
  std::map<double, std::unique_ptr<PriceLevel>> sell_orders;
  mutable std::mutex                            mtx; // Mutex to protect access to the order book

  bool canMatch(double buy_price, double sell_price);
  int  next_order_id = 1;
};

#endif // ORDER_BOOK_HH
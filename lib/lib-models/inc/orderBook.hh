#ifndef ORDER_BOOK_HH
#define ORDER_BOOK_HH

#include "order.hh"
#include "priceLevel.hh"
#include <map>
#include <shared_mutex>
#include <nlohmann/json.hpp>
#include "metrics.hh"

class OrderBook
{
  public:
  void addOrder(std::unique_ptr<Order> new_order);
  void match();
  int  getNextOrderId() { return next_order_id++; }

  bool isEmpty() { return buy_orders.empty() && sell_orders.empty(); }

  double getBestBid()
  {
    if(buy_orders.empty()) return 0.0;
    // return the highest buy price
    return buy_orders.rbegin()->first;
  }

  nlohmann::json toJson()
  {
    std::shared_lock lock(mtx);
    nlohmann::json   json;
    {
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

  static std::unique_ptr<OrderBook> createFromJson(const nlohmann::json &orderBookData)
  {
    auto orderBook = std::make_unique<OrderBook>();

    // Deserialize buy orders
    for(const auto &buyLevelData : orderBookData["buy_orders"])
      {
        auto buyLevel                               = PriceLevel::createFromJson(buyLevelData);
        orderBook->buy_orders[buyLevel->getPrice()] = std::move(buyLevel);
      }

    // Deserialize sell orders
    for(const auto &sellLevelData : orderBookData["sell_orders"])
      {
        auto sellLevel                                = PriceLevel::createFromJson(sellLevelData);
        orderBook->sell_orders[sellLevel->getPrice()] = std::move(sellLevel);
      }

    return orderBook;
  }

  void clear()
  {
    std::unique_lock lock(mtx);
    buy_orders.clear();
    sell_orders.clear();
  }

  int totalOrders()
  {
    std::shared_lock lock(mtx);
    // for each priceLevel use getSize() to get the number of orders
    int total = 0;
    for(const auto &[price, level] : buy_orders) total += level->getSize();
    for(const auto &[price, level] : sell_orders) total += level->getSize();
    return total;
  }

  // Getters
  std::map<double, std::unique_ptr<PriceLevel>> &getBuyOrders() { return buy_orders; }
  std::map<double, std::unique_ptr<PriceLevel>> &getSellOrders() { return sell_orders; }

  private:
  std::map<double, std::unique_ptr<PriceLevel>> buy_orders;
  std::map<double, std::unique_ptr<PriceLevel>> sell_orders;
  std::shared_mutex                             mtx; // Mutex to protect access to the order book

  bool canMatch(double buy_price, double sell_price);
  int  next_order_id = 1;
};

#endif // ORDER_BOOK_HH
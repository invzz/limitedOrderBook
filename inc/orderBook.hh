#ifndef ORDER_BOOK_HH
#define ORDER_BOOK_HH

#include "order.hh"
#include "priceLevel.hh"
#include <map>
#include <memory>
#include <mutex>
#include "metrics.hh"

class OrderBook
{
  public:
  void addOrder(std::unique_ptr<Order> new_order);
  void match(std::unordered_map<int, Metrics *> metricsMap);
  int  getNextOrderId() { return next_order_id++; }

  bool isEmpty() { return buy_orders.empty() && sell_orders.empty(); }

  double getBestBid()
  {
    if(buy_orders.empty()) return 0.0;
    // return the highest buy price
    return buy_orders.rbegin()->first;
  }

  // Getters
  std::map<double, std::unique_ptr<PriceLevel>> &getBuyOrders() { return buy_orders; }
  std::map<double, std::unique_ptr<PriceLevel>> &getSellOrders() { return sell_orders; }

  private:
  std::map<double, std::unique_ptr<PriceLevel>> buy_orders;
  std::map<double, std::unique_ptr<PriceLevel>> sell_orders;
  std::mutex                                    mtx; // Mutex to protect access to the order book

  bool canMatch(double buy_price, double sell_price);
  int  next_order_id = 1;
};

#endif // ORDER_BOOK_HH
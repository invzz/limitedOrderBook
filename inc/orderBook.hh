#ifndef ORDER_BOOK_HH
#define ORDER_BOOK_HH

#include "order.hh"
#include "priceLevel.hh"
#include <map>
#include <memory>
#include <mutex>

class OrderBook
{
  public:
  void addOrder(std::unique_ptr<Order> new_order);
  void match(std::map<int, double> &gainsLosses);
  int  getNextOrderId() { return next_order_id++; }

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
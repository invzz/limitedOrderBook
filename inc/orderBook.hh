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
  void addOrder(Order *new_order);
  void matchOrder(Order *new_order);
  void match();
  bool canMatch(double buy_price, double sell_price);
  void matchBuySellOrders(PriceLevel *buy_level, PriceLevel *sell_level);
  // Getters
  std::map<double, std::unique_ptr<PriceLevel>> &getBuyOrders()
  {
    return buy_orders;
  }
  std::map<double, std::unique_ptr<PriceLevel>> &getSellOrders()
  {
    return sell_orders;
  }

  private:
  std::map<double, std::unique_ptr<PriceLevel>> buy_orders;
  std::map<double, std::unique_ptr<PriceLevel>> sell_orders;
  std::mutex                                    mtx;
};

#endif // ORDER_BOOK_HH

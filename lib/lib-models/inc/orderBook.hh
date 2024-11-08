#ifndef ORDER_BOOK_HH
#define ORDER_BOOK_HH

#include "order.hh"
#include "priceLevel.hh"
#include <map>
#include <shared_mutex>
#include <nlohmann/json.hpp>
#include <trade.hh>
class OrderBook
{
  public:
  void                                addOrder(std::shared_ptr<Order> new_order); // Change to shared_ptr
  std::vector<std::shared_ptr<Trade>> match(int tick = 0);
  int                                 getNextOrderId() { return next_order_id++; }
  bool                                isEmpty() { return bids.empty() && asks.empty(); }
  nlohmann::json                      toJson();
  std::vector<std::shared_ptr<Order>> getBestBid(); // Change to shared_ptr
  std::vector<std::shared_ptr<Order>> getBestAsk(); // Change to shared_ptr

  static std::shared_ptr<OrderBook> fromJson(const nlohmann::json &orderBookData);

  void clear()
  {
    std::unique_lock lock(asks_mtx);
    std::unique_lock lock2(bids_mtx);
    bids.clear();
    asks.clear();
  }

  size_t totalOrders()
  {
    std::unique_lock lock(asks_mtx);
    std::unique_lock lock2(bids_mtx);
    size_t           total = 0;
    for(const auto &[price, level] : bids) total += level->getSize();
    for(const auto &[price, level] : asks) total += level->getSize();
    return total;
  }

  std::map<double, std::shared_ptr<PriceLevel>> &getBids() { return bids; } // Change to shared_ptr
  std::map<double, std::shared_ptr<PriceLevel>> &getAsks() { return asks; } // Change to shared_ptr

  protected:
  std::map<double, std::shared_ptr<PriceLevel>> bids = std::map<double, std::shared_ptr<PriceLevel>>(); // Change to shared_ptr
  std::map<double, std::shared_ptr<PriceLevel>> asks = std::map<double, std::shared_ptr<PriceLevel>>(); // Change to shared_ptr

  private:
  std::shared_mutex bids_mtx;
  std::shared_mutex asks_mtx; // Mutex to protect access to the order book
  bool              canMatch(double buy_price, double sell_price);
  int               next_order_id = 1;
};

#endif // ORDER_BOOK_HH

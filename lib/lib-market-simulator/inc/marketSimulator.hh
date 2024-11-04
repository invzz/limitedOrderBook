// orderBookServer.hh
#ifndef ORDER_BOOK_SERVER_HH
#define ORDER_BOOK_SERVER_HH
#include <vector>
#include <memory>
#include "bot.hh"
#include "orderBook.hh"

class MarketSimulator
{
  public:
  void       tick(int numTicks = 1, int tickDurationMs = 100);
  void       logMetrics();
  void       addBot(std::unique_ptr<Bot> bot);
  OrderBook &getOrderBook() { return orderBook; }
  void       addOrder(std::unique_ptr<Order> order);
  int        getCurrentTick() { return current_tick; }
  void       resetCurrentTick() { current_tick = 0; }
  void       incrementCurrentTick() { current_tick++; }

  private:
  std::unordered_map<int, std::unique_ptr<Bot>> bots;
  std::unordered_map<int, Metrics *>            metricsMap; // Map userId to Metrics
  OrderBook                                     orderBook;
  std::mutex                                    mtx;
  int                                           current_tick = 0;
};
#endif // ORDER_BOOK_SERVER_HH
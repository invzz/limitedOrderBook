// orderBookServer.hh
#ifndef ORDER_BOOK_SERVER_HH
#define ORDER_BOOK_SERVER_HH
#include <vector>
#include <memory>
#include "bot.hh"
#include "orderBook.hh"

class OrderBookService
{
  public:
  void       tick(int numTicks = 1, int tickDurationMs = 100);
  void       logMetrics();
  void       addBot(std::unique_ptr<Bot> bot);
  OrderBook &getOrderBook() { return orderBook; }
  void       addOrder(std::unique_ptr<Order> order);

  private:
  std::unordered_map<int, std::unique_ptr<Bot>> bots;
  std::unordered_map<int, Metrics *>            metricsMap; // Map userId to Metrics
  OrderBook                                     orderBook;
  std::mutex                                    mtx;
};
#endif // ORDER_BOOK_SERVER_HH
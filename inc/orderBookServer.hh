#ifndef ORDER_BOOK_SERVER_HH
#define ORDER_BOOK_SERVER_HH
#include "orderBook.hh"
#include "order.hh"
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <map>

class OrderBookServer
{
  public:
  explicit OrderBookServer();

  // Method for adding orders to the order book
  void addOrder(std::unique_ptr<Order> order);
  void match() { orderBook.match(userPositions); }

  void printPositions()
  {
    for(auto position : userPositions) { spdlog::info(LOG_INFO_FMT + LOG_POSITION, position.first, position.second); }
  }

  void printPosition(int userId) { spdlog::info(LOG_INFO_FMT + LOG_POSITION, userId, userPositions[userId]); }

  std::map<int, double> getUserPositions() { return userPositions; }
  double                getUserPosition(int userId) { return userPositions[userId]; }
  OrderBook            &getOrderBook() { return orderBook; }

  private:
  const std::string     LOG_INFO_FMT = "[ OrderBookServer ] :: ";
  const std::string     LOG_POSITION = "[ Position ] >> [ USER {:03} ] >> [ profit / loss ] :: $ {:.2f}";

  OrderBook             orderBook;
  std::map<int, double> userPositions; // userId -> gain/loss
  std::mutex            mtx;           // Mutex to ensure thread-safe access to the order book
};

#endif // ORDER_BOOK_SERVER_HH
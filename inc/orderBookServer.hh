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
  void printBotPositions()
  {
    for(auto position : userPositions) { spdlog::info("Bot ID: {:03}, Gain/Loss: {:.2f}", position.first, position.second); }
  }

  private:
  OrderBook             orderBook;
  std::map<int, double> userPositions; // userId -> gain/loss
  std::mutex            mtx;           // Mutex to ensure thread-safe access to the order book
};

#endif // ORDER_BOOK_SERVER_HH
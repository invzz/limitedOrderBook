#ifndef ORDER_BOOK_SERVER_HH
#define ORDER_BOOK_SERVER_HH
#include "orderBook.hh"
#include "order.hh"
#include <memory>
#include <mutex>

class OrderBookServer
{
  public:
  explicit OrderBookServer();

  // Method for adding orders to the order book
  void addOrder(std::unique_ptr<Order> order);
  void match() { orderBook.match(); }
  

  private:
  OrderBook  orderBook;
  std::mutex mtx; // Mutex to ensure thread-safe access to the order book
};

#endif // ORDER_BOOK_SERVER_HH
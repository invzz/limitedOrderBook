#include "orderBookServer.hh"

OrderBookServer::OrderBookServer() : orderBook() {}

void OrderBookServer::addOrder(std::unique_ptr<Order> order)
{
  order->setId(orderBook.getNextOrderId());
  int       id         = order->getId();
  double    price      = order->getPrice();
  int       quantity   = order->getQuantity();
  OrderType type       = order->getType();
  int       userId     = order->getUserId();
  auto      stringType = (type == OrderType::BUY) ? "BUY " : "SELL";

  std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access
  orderBook.addOrder(std::move(order));  // Release ownership of the order
  spdlog::debug("[ OrderBook ] :: User {} >> [ {} Order ] < {:^5} > {:^.2f} x {:^4} Added .", userId, stringType, id, price, quantity);
  match();
}
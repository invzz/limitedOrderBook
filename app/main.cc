#include "orderBook.hh"
#include "spdlog/spdlog.h"
#include <iostream>

int main()
{
  spdlog::set_level(spdlog::level::info);
  OrderBook order_book;

  // Adding sample orders for testing
  Order buy_order1(1, OrderType::BUY, 100.0, 10);
  Order sell_order1(2, OrderType::SELL, 50.0, 10);
  Order sell_order2(3, OrderType::SELL, 20.0, 10);

  order_book.addOrder(&buy_order1);
  order_book.addOrder(&sell_order1);
  order_book.addOrder(&sell_order2);

  return 0;
}

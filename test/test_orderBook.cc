#include "orderBook.hh"
#include <gtest/gtest.h>
#include <algorithm>
#include <spdlog/spdlog.h>

TEST(OrderBookTest, AddMultipleOrders)
{
  OrderBook orderBook;

  // Create multiple buy and sell orders
  Order buyOrder1(1, OrderType::BUY, 100, 10);  // Higher-priced order
  Order buyOrder2(2, OrderType::BUY, 95, 5);    // Exact price match
  Order sellOrder1(3, OrderType::SELL, 95, 10); // Partial match with buyOrder2
  Order sellOrder2(4, OrderType::SELL, 105, 5); // Won't be matched

  // Add orders to the order book
  orderBook.addOrder(&buyOrder1);
  orderBook.addOrder(&buyOrder2);
  orderBook.addOrder(&sellOrder1);
  orderBook.addOrder(&sellOrder2);

  // Call match after adding orders
  orderBook.match();

  // Verify quantities directly on the Order instances
  EXPECT_EQ(buyOrder1.getQuantity(), 5);  // buyOrder1 partially filled
  EXPECT_EQ(buyOrder2.getQuantity(), 0);  // buyOrder2 fully filled
  EXPECT_EQ(sellOrder1.getQuantity(), 0); // sellOrder1 fully filled
  EXPECT_EQ(sellOrder2.getQuantity(), 5); // sellOrder2 remains untouched
}

TEST(OrderBookTest, PartialMatchOrders)
{
  OrderBook orderBook;
  Order     buyOrder(1, OrderType::BUY, 100, 10);
  Order     sellOrder(2, OrderType::SELL, 100, 5);

  orderBook.addOrder(&buyOrder);
  orderBook.addOrder(&sellOrder);

  orderBook.match(); // This should only partially match

  EXPECT_EQ(orderBook.getBuyOrders().size(), 1);  // Buy order should remain
  EXPECT_EQ(orderBook.getSellOrders().size(), 0); // Sell order should be filled
  EXPECT_EQ(buyOrder.getQuantity(), 5); // Buy order should have 5 remaining
}

TEST(OrderBookTest, NoMatch)
{
  OrderBook orderBook;
  Order     buyOrder(1, OrderType::BUY, 100, 10);
  Order     sellOrder(2, OrderType::SELL, 110, 5);

  orderBook.addOrder(&buyOrder);
  orderBook.addOrder(&sellOrder);

  EXPECT_EQ(orderBook.getBuyOrders().size(), 1);  // Buy order should remain
  EXPECT_EQ(orderBook.getSellOrders().size(), 1); // Sell order should remain
}

TEST(OrderBookTest, MatchMultipleOrders)
{
  OrderBook orderBook;

  Order buyOrder1(1, OrderType::BUY, 95, 10);   // Price = 95, Quantity = 10
  Order sellOrder1(2, OrderType::SELL, 100, 5); // Price = 100, Quantity = 5
  Order sellOrder2(3, OrderType::SELL, 95, 5);  // Price = 95, Quantity = 5

  orderBook.addOrder(&buyOrder1);
  orderBook.addOrder(&sellOrder1);
  orderBook.addOrder(&sellOrder2);

  EXPECT_EQ(orderBook.getBuyOrders().size(),
            1); // buyOrder1 should remain (5 units left)
  EXPECT_EQ(orderBook.getBuyOrders().begin()->second->getOrders().size(),
            1); // buyOrder1 should still exist
  EXPECT_EQ(orderBook.getBuyOrders()
              .begin()
              ->second->getOrders()
              .front()
              ->getQuantity(),
            5); // remaining quantity should be 5
  EXPECT_EQ(orderBook.getSellOrders().size(), 1); // sellOrder1 should remain
  EXPECT_EQ(orderBook.getSellOrders().begin()->second->getOrders().size(),
            1); // sellOrder1 should still exist
  EXPECT_EQ(orderBook.getSellOrders()
              .begin()
              ->second->getOrders()
              .front()
              ->getQuantity(),
            5); // remaining quantity should be 5
}

TEST(OrderBookTest, HandleNegativeQuantity)
{
  OrderBook orderBook;
  Order     buyOrder(1, OrderType::BUY, 100, 10);
  Order     sellOrder(2, OrderType::SELL, 100, 10);

  orderBook.addOrder(&buyOrder);
  orderBook.addOrder(&sellOrder);

  EXPECT_EQ(buyOrder.getQuantity(), 0);  // Buy order should be filled
  EXPECT_EQ(sellOrder.getQuantity(), 0); // Sell order should be filled
}

TEST(OrderBookTest, RemoveFilledOrders)
{
  OrderBook orderBook;
  Order     buyOrder(1, OrderType::BUY, 100, 10);
  Order     sellOrder(2, OrderType::SELL, 100, 10);

  orderBook.addOrder(&buyOrder);
  orderBook.addOrder(&sellOrder);

  EXPECT_EQ(orderBook.getBuyOrders().size(), 0);  // No buy orders left
  EXPECT_EQ(orderBook.getSellOrders().size(), 0); // No sell orders left
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
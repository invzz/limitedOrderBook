#include "orderBook.hh"
#include <gtest/gtest.h>
#include <algorithm>
#include <spdlog/spdlog.h>

// Create a simple Order for testing
std::unique_ptr<Order> createOrder(OrderType type, double price, int quantity, int userId) { return std::make_unique<Order>(type, price, quantity, userId); }

TEST(OrderBookTest, AddMultipleOrders)
{
  OrderBook orderBook;

  // Add BUY orders
  orderBook.addOrder(createOrder(OrderType::BUY, 100.0, 10, 1));
  orderBook.addOrder(createOrder(OrderType::BUY, 101.0, 5, 2));

  // Add SELL orders
  orderBook.addOrder(createOrder(OrderType::SELL, 102.0, 7, 3));
  orderBook.addOrder(createOrder(OrderType::SELL, 103.0, 10, 4));

  // Check the number of buy and sell orders
  ASSERT_EQ(orderBook.getBuyOrders().size(), 2);
  ASSERT_EQ(orderBook.getSellOrders().size(), 2);
}

TEST(OrderBookTest, PartialMatchOrders)
{
  OrderBook orderBook;

  // Add orders
  orderBook.addOrder(createOrder(OrderType::BUY, 100.0, 10, 1)); // User 1 wants to buy 10 at 100
  orderBook.addOrder(createOrder(OrderType::SELL, 100.0, 5, 2)); // User 2 sells 5 at 100

  // Execute matching
  std::map<int, double> gainsLosses; // Used to track gains/losses
  orderBook.match(gainsLosses);

  // Verify the quantities after matching
  ASSERT_EQ(orderBook.getBuyOrders()[100]->getOrders().front()->getQuantity(), 5); // User 1 should have 5 remaining
  ASSERT_TRUE(orderBook.getSellOrders().empty());                                  // User 2 should be filled
}

TEST(OrderBookTest, NoMatch)
{
  OrderBook orderBook;

  // Add BUY and SELL orders that cannot match
  orderBook.addOrder(createOrder(OrderType::BUY, 100.0, 10, 1));  // User 1 wants to buy 10 at 100
  orderBook.addOrder(createOrder(OrderType::SELL, 101.0, 10, 2)); // User 2 sells 10 at 101

  std::map<int, double> gainsLosses;
  orderBook.match(gainsLosses); // No matching should occur

  // Check the orders still exist
  ASSERT_EQ(orderBook.getBuyOrders().size(), 1);
  ASSERT_EQ(orderBook.getSellOrders().size(), 1);
}

TEST(OrderBookTest, MatchMultipleOrders)
{
  OrderBook orderBook;

  // Add orders
  orderBook.addOrder(createOrder(OrderType::BUY, 100.0, 10, 1));  // User 1
  orderBook.addOrder(createOrder(OrderType::BUY, 100.0, 5, 2));   // User 2
  orderBook.addOrder(createOrder(OrderType::SELL, 100.0, 10, 3)); // User 3

  // Execute matching
  std::map<int, double>  gainsLosses;
  orderBook.match(gainsLosses);

  // Verify the quantities after matching
  ASSERT_EQ(orderBook.getBuyOrders().size(), 1); // Only User 2 should have remaining orders
  ASSERT_EQ(orderBook.getBuyOrders()[100]->getOrders().front()->getQuantity(), 5);
  ASSERT_TRUE(orderBook.getSellOrders().empty()); // User 3 should be filled

  ASSERT_EQ(gainsLosses[1], -1000); // User 1 should have lost 50
  ASSERT_EQ(gainsLosses[2], 0);   // User 2 should have no gain/loss
  ASSERT_EQ(gainsLosses[3], 1000);  // User 3 should have gained 50
}

TEST(OrderBookTest, HandleNegativeQuantity)
{
  OrderBook orderBook;

  // Add a buy order with a negative quantity
  orderBook.addOrder(createOrder(OrderType::BUY, 100.0, -5, 1)); // Invalid order

  // Check the orders still exist (should not have added)
  ASSERT_EQ(orderBook.getBuyOrders().size(), 0);
}

TEST(OrderBookTest, RemoveFilledOrders)
{
  OrderBook orderBook;

  // Add orders
  orderBook.addOrder(createOrder(OrderType::BUY, 100.0, 10, 1));
  orderBook.addOrder(createOrder(OrderType::SELL, 100.0, 10, 2));

  // Execute matching
  std::map<int, double> gainsLosses;
  orderBook.match(gainsLosses);

  // Verify that the orders are removed
  ASSERT_EQ(orderBook.getBuyOrders().size(), 0);  // Should be empty
  ASSERT_EQ(orderBook.getSellOrders().size(), 0); // Should be empty
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
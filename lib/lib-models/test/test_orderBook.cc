#include "orderBook.hh"
#include <gtest/gtest.h>
#include <algorithm>
#include <spdlog/spdlog.h>

// Create a simple Order for testing
std::unique_ptr<Order> createOrder(OrderType type, double price, int quantity, int userId)
{
  return std::make_unique<Order>(type, price, quantity, userId);
}

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
  OrderBook                          orderBook; // Used to track metrix
  std::unordered_map<int, Metrics *> metricsMap;

  auto order1 = createOrder(OrderType::BUY, 100.0, 10, 1);
  auto order2 = createOrder(OrderType::SELL, 100.0, 5, 2);

  // Set the order IDs
  order1->setId(1);
  order2->setId(2);
  metricsMap[1] = new Metrics();
  metricsMap[2] = new Metrics();

  // Add BUY and SELL orders
  orderBook.addOrder(std::move(order1));
  orderBook.addOrder(std::move(order2));

  // Execute matching
  orderBook.match(0, metricsMap);

  // // Verify the quantities after matching
  ASSERT_EQ(orderBook.getBuyOrders()[100]->getOrders().front()->getQuantity(),
            5);                                   // User 1 should have 5 remaining
  ASSERT_TRUE(orderBook.getSellOrders().empty()); // User 2 should be filled
}

TEST(OrderBookTest, NoMatch)
{
  OrderBook                           orderBook;
  std::unordered_map<int, Metrics *>  metricsMap;
  std::vector<std::unique_ptr<Order>> orders;

  orders.emplace_back(createOrder(OrderType::BUY, 100.0, 10, 1));
  orders.emplace_back(createOrder(OrderType::SELL, 101.0, 10, 2));

  for(auto i = 1; i <= orders.size(); i++)
    {
      metricsMap[i] = new Metrics();
      orders[i - 1]->setId(i);
      orderBook.addOrder(std::move(orders[i - 1]));
    }

  // Execute matching
  orderBook.match(0, metricsMap);

  // Check the orders still exist
  ASSERT_EQ(orderBook.getBuyOrders().size(), 1);
  ASSERT_EQ(orderBook.getSellOrders().size(), 1);
}

TEST(OrderBookTest, MatchMultipleOrders)
{
  // Used to track metrix
  std::unordered_map<int, Metrics *>  metricsMap;
  std::vector<std::unique_ptr<Order>> orders;
  OrderBook                           orderBook;

  orders.emplace_back(createOrder(OrderType::BUY, 100.0, 10, 1));
  orders.emplace_back(createOrder(OrderType::BUY, 100.0, 5, 2));
  orders.emplace_back(createOrder(OrderType::SELL, 100.0, 10, 3));

  for(auto i = 1; i <= orders.size(); i++)
    {
      metricsMap[i] = new Metrics();
      orders[i - 1]->setId(i);
      orderBook.addOrder(std::move(orders[i - 1]));
    }

  // Execute matching
  orderBook.match(0, metricsMap);

  // Verify the quantities after matching
  ASSERT_EQ(orderBook.getBuyOrders().size(), 1); // Only User 2 should have remaining orders
  ASSERT_EQ(orderBook.getBuyOrders()[100]->getOrders().front()->getQuantity(), 5);
  ASSERT_TRUE(orderBook.getSellOrders().empty()); // User 3 should be filled

  ASSERT_EQ(metricsMap[1]->getTotalProfit(), -1000); // User 1 should have lost 1000
  ASSERT_EQ(metricsMap[2]->getTotalProfit(), 0);     // User 2 should have no gain/loss
  ASSERT_EQ(metricsMap[3]->getTotalProfit(), 1000);  // User 3 should have gained 1000
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
  // Used to track metrix
  std::unordered_map<int, Metrics *>  metricsMap;
  std::vector<std::unique_ptr<Order>> orders;
  OrderBook                           orderBook;

  // Add orders
  orders.emplace_back(createOrder(OrderType::BUY, 100.0, 10, 1));
  orders.emplace_back(createOrder(OrderType::SELL, 100.0, 10, 2));

  for(auto i = 1; i <= orders.size(); i++)
    {
      metricsMap[i] = new Metrics();
      orders[i - 1]->setId(i);
      orderBook.addOrder(std::move(orders[i - 1]));
    }

  orderBook.match(0, metricsMap);

  // Verify that the orders are removed
  ASSERT_EQ(orderBook.getBuyOrders().size(), 0);  // Should be empty
  ASSERT_EQ(orderBook.getSellOrders().size(), 0); // Should be empty
}

int main(int argc, char **argv)
{
  spdlog::set_level(spdlog::level::critical);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
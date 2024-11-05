#include "marketSimulator.hh"
#include "buyerBot.hh"
#include "sellerBot.hh"
#include "order.hh"
#include <gtest/gtest.h>
#include <memory>

class MarketSimulatorTest : public ::testing::Test
{
  protected:
  MarketSimulator simulator;

  void SetUp() override
  {
    simulator.resetCurrentTick(); // Ensure starting from tick 0
  }
};

TEST_F(MarketSimulatorTest, BuyerBotPlacesOrderWithinPriceRange)
{
  auto buyerBot = std::make_unique<BuyerBot>("BuyerBot1", &simulator, 10.0, 50.0, 100);
  simulator.addBot(std::move(buyerBot));

  simulator.tick(1); // Run one tick for the buyer bot to place an order

  auto       &orderBook = simulator.getOrderBook();
  const auto &buyOrders = orderBook.getBuyOrders();
  ASSERT_FALSE(buyOrders.empty()) << "Expected at least one buy order in the order book";

  // Check that the placed order is within the buyer's defined price range
  double minPrice = 10.0, maxPrice = 50.0;
  double placedPrice = buyOrders.begin()->first;
  EXPECT_GE(placedPrice, minPrice) << "Order price is below the minimum specified price";
  EXPECT_LE(placedPrice, maxPrice) << "Order price is above the maximum specified price";
}

TEST_F(MarketSimulatorTest, SellerBotPlacesOrderWithinAdjustedPriceRange)
{
  auto sellerBot = std::make_unique<SellerBot>("SellerBot1", &simulator, 5.0, 30.0, 100);
  simulator.addBot(std::move(sellerBot));

  simulator.tick(1); // Run one tick for the seller bot to place an order

  auto       &orderBook  = simulator.getOrderBook();
  const auto &sellOrders = orderBook.getSellOrders();
  ASSERT_FALSE(sellOrders.empty()) << "Expected at least one sell order in the order book";

  // Check that the placed order is within the seller's defined price range
  double minPrice = 5.0, maxPrice = 30.0;
  double placedPrice = sellOrders.begin()->first;
  EXPECT_GE(placedPrice, minPrice) << "Order price is below the minimum specified price";
  EXPECT_LE(placedPrice, maxPrice) << "Order price is above the maximum specified price";
}

TEST_F(MarketSimulatorTest, BuyerBotOrdersTargetLowestSellPrice)
{
  // Simulate an initial sell order to set a target for the buyer bot
  simulator.getOrderBook().addSellOrder(std::make_unique<Order>(OrderType::SELL, 12.0, 10, 1));
  auto buyerBot = std::make_unique<BuyerBot>("BuyerBot2", &simulator, 10.0, 20.0, 50);
  simulator.addBot(std::move(buyerBot));

  simulator.tick(1); // Run one tick for buyer bot to observe sell orders and place buy order

  auto       &orderBook = simulator.getOrderBook();
  const auto &buyOrders = orderBook.getBuyOrders();
  ASSERT_FALSE(buyOrders.empty()) << "Expected at least one buy order";

  // Verify that buyer bot placed order near or above the lowest sell price to remain competitive
  double targetPrice = 12.0;
  double placedPrice = buyOrders.begin()->first;
  EXPECT_GE(placedPrice, targetPrice) << "BuyerBot did not meet or exceed lowest sell price";
}

TEST_F(MarketSimulatorTest, SellerBotOrdersTargetHighestBuyPrice)
{
  // Simulate an initial buy order to set a target for the seller bot
  simulator.getOrderBook().addBuyOrder(std::make_unique<Order>(OrderType::BUY, 18.0, 20, 2));
  auto sellerBot = std::make_unique<SellerBot>("SellerBot2", &simulator, 10.0, 25.0, 80);
  simulator.addBot(std::move(sellerBot));

  simulator.tick(1); // Run one tick for seller bot to observe buy orders and place sell order

  auto       &orderBook  = simulator.getOrderBook();
  const auto &sellOrders = orderBook.getSellOrders();
  ASSERT_FALSE(sellOrders.empty()) << "Expected at least one sell order";

  // Verify that seller bot placed order near or just below the highest buy price to remain
  // competitive
  double targetPrice = 18.0;
  double placedPrice = sellOrders.begin()->first;
  EXPECT_LE(placedPrice, targetPrice) << "SellerBot did not meet or fall below highest buy price";
}

TEST_F(MarketSimulatorTest, BuyerBotPlacesOrderWithRandomQuantity)
{
  auto buyerBot = std::make_unique<BuyerBot>("BuyerBot3", &simulator, 10.0, 50.0, 50);
  simulator.addBot(std::move(buyerBot));

  simulator.tick(1); // Run one tick to place the order

  auto       &orderBook = simulator.getOrderBook();
  const auto &buyOrders = orderBook.getBuyOrders();
  ASSERT_FALSE(buyOrders.empty()) << "Expected at least one buy order";

  const auto &priceLevel = buyOrders.begin()->second;
  ASSERT_FALSE(priceLevel->getOrders().empty()) << "Expected at least one order at the price level";

  // Verify that the quantity of the placed order is within the buyer's specified range
  const auto &placedOrder = priceLevel->getOrders().front();
  EXPECT_GE(placedOrder->getQuantity(), 1);
  EXPECT_LE(placedOrder->getQuantity(), 50) << "Order quantity exceeds maximum allowed";
}

TEST_F(MarketSimulatorTest, SellerBotPlacesOrderWithRandomQuantity)
{
  auto sellerBot = std::make_unique<SellerBot>("SellerBot3", &simulator, 5.0, 35.0, 40);
  simulator.addBot(std::move(sellerBot));

  simulator.tick(1); // Run one tick to place the order

  auto       &orderBook  = simulator.getOrderBook();
  const auto &sellOrders = orderBook.getSellOrders();
  ASSERT_FALSE(sellOrders.empty()) << "Expected at least one sell order";

  const auto &priceLevel = sellOrders.begin()->second;
  ASSERT_FALSE(priceLevel->getOrders().empty()) << "Expected at least one order at the price level";

  // Verify that the quantity of the placed order is within the seller's specified range
  const auto &placedOrder = priceLevel->getOrders().front();
  EXPECT_GE(placedOrder->getQuantity(), 1);
  EXPECT_LE(placedOrder->getQuantity(), 40) << "Order quantity exceeds maximum allowed";
}
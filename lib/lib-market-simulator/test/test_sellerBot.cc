#include "sellerBot.hh"
#include "marketSimulator.hh"
#include "order.hh"
#include "orderBook.hh"
#include <gtest/gtest.h>
#include <memory>

class SellerBotTest : public ::testing::Test
{
  protected:
  MarketSimulator            simulator;
  std::unique_ptr<SellerBot> sellerBot;

  void SetUp() override
  {
    // Initialize the simulator and SellerBot with specified min and max price values
    simulator.initializeMarket();
    sellerBot = std::make_unique<SellerBot>("SellerBot1", &simulator, 1.0, 20.0, 100);
  }
};

TEST_F(SellerBotTest, SetsPriceRangeBasedOnBuyOrders)
{
  auto &orderBook = simulator.getOrderBook();

  // Add some buy orders to influence SellerBot's price range
  orderBook.addOrder(std::make_unique<Order>(OrderType::BUY, 15.0, 10, "Buyer1"));
  orderBook.addOrder(std::make_unique<Order>(OrderType::BUY, 18.0, 5, "Buyer2"));

  // Run the SellerBot to adjust its price range
  sellerBot->run();

  // Check if min_price and max_price were adjusted based on average buy price
  double averageBuyPrice = ((15.0 * 10) + (18.0 * 5)) / 15; // TotalPrice / TotalQuantity

  double expectedMinPrice = averageBuyPrice * 0.95;
  double expectedMaxPrice = averageBuyPrice * 1.05;

  EXPECT_GE(sellerBot->getMinPrice(), expectedMinPrice);
  EXPECT_LE(sellerBot->getMaxPrice(), expectedMaxPrice);
}

TEST_F(SellerBotTest, SkipsTurnIfInvalidPriceRange)
{
  sellerBot->setMinPrice(20.0);
  sellerBot->setMaxPrice(15.0); // Set min_price greater than max_price to create an invalid range

  // Run the SellerBot to see if it skips its turn
  size_t initialOrderCount = simulator.getOrderBook().getSellOrders().size();
  sellerBot->run();

  // No new order should be placed due to the invalid price range
  EXPECT_EQ(simulator.getOrderBook().getSellOrders().size(), initialOrderCount);
}

TEST_F(SellerBotTest, PlacesOrderWithinValidPriceRange)
{
  auto &orderBook = simulator.getOrderBook();

  // Add a few buy orders to create a competitive environment
  orderBook.addOrder(std::make_unique<Order>(OrderType::BUY, 14.0, 20, "Buyer3"));
  orderBook.addOrder(std::make_unique<Order>(OrderType::BUY, 16.0, 15, "Buyer4"));

  // Run the SellerBot, expecting it to adjust and place an order
  sellerBot->run();

  // Check that a sell order was placed within the adjusted min and max price range
  auto sellOrders = orderBook.getSellOrders();
  ASSERT_FALSE(sellOrders.empty());

  // Retrieve the price of the placed sell order
  double placedPrice = sellOrders.begin()->first; // Assuming getSellOrders() returns a map

  EXPECT_GE(placedPrice, sellerBot->getMinPrice());
  EXPECT_LE(placedPrice, sellerBot->getMaxPrice());
}

TEST_F(SellerBotTest, GeneratesOrderWithRandomQuantity)
{
  auto &orderBook = simulator.getOrderBook();

  // Run SellerBot to place an order
  sellerBot->run();

  // Check that an order was created
  auto sellOrders = orderBook.getSellOrders();
  ASSERT_FALSE(sellOrders.empty());

  // Verify the quantity is within the bot’s allowable range
  const auto &placedOrder = sellOrders.begin()->second->getOrders().front();
  EXPECT_GE(placedOrder->getQuantity(), 1);
  EXPECT_LE(placedOrder->getQuantity(), 100);
}
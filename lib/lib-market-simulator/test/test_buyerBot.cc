#include "buyerBot.hh"
#include "marketSimulator.hh"
#include "order.hh"
#include "orderBook.hh"
#include <gtest/gtest.h>

class BuyerBotTest : public ::testing::Test
{
  protected:
  MarketSimulator           simulator;
  std::unique_ptr<BuyerBot> buyerBot;

  void SetUp() override
  {
    // Set up the simulator and BuyerBot with specified range values
    simulator.initializeMarket();
    buyerBot = std::make_unique<BuyerBot>("BuyerBot1", &simulator, 1.0, 10.0, 100);
  }
};

TEST_F(BuyerBotTest, CreatesOrderWithExpectedPriceAndQuantityRange)
{
  auto buyOrder = buyerBot->createBuyOrder();

  // Check that the buy order price and quantity are within expected ranges
  EXPECT_GE(buyOrder->getPrice(), 1.0);
  EXPECT_LE(buyOrder->getPrice(), 10.0);
  EXPECT_GE(buyOrder->getQuantity(), 1);
  EXPECT_LE(buyOrder->getQuantity(), 100);
}

TEST_F(BuyerBotTest, AdjustsPriceBasedOnSellOrders)
{
  // Add a sell order to the order book
  auto &orderBook = simulator.getOrderBook();
  orderBook.addOrder(std::make_unique<Order>(OrderType::SELL, 5.0, 50, "Seller1"));

  // Run the buyer bot to place an order based on the existing sell order
  buyerBot->run();

  // Check that a new order is added
  auto buyOrders = orderBook.getBuyOrders();
  ASSERT_FALSE(buyOrders.empty());

  // Verify the buy order's price is set to be slightly above the lowest sell order
  double expectedPrice = 5.05;                      // Target price slightly above the lowest sell
  auto   placedOrder   = buyOrders.begin()->second; // Assuming we can get the first buy order
  EXPECT_NEAR(placedOrder->getPrice(), expectedPrice,
              0.01); // Allowing for minor floating-point deviation
}

TEST_F(BuyerBotTest, PlacesOrderWhenNoSellOrdersExist)
{
  // Ensure no sell orders exist in the market
  auto &orderBook = simulator.getOrderBook();
  EXPECT_TRUE(orderBook.getSellOrders().empty());

  // Run the buyer bot to place an order
  buyerBot->run();

  // Check that an order is placed
  auto buyOrders = orderBook.getBuyOrders();
  ASSERT_EQ(buyOrders.size(), 1);

  // Verify the placed order price is the maximum price, as there were no sell orders to undercut
  auto placedOrder = buyOrders.begin()->second;
  EXPECT_EQ(placedOrder->getPrice(), 10.0); // Assumes max_price is 10.0
}
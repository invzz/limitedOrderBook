#include "priceLevel.hh"
#include <gtest/gtest.h>
// Create a simple Order for testing
std::unique_ptr<Order> createOrder(OrderType type, double price, int quantity, int userId) { return std::make_unique<Order>(type, price, quantity, userId); }

TEST(PriceLevelTest, PriceLevelCreation)
{
  PriceLevel priceLevel(100);
  EXPECT_EQ(priceLevel.getPrice(), 100);
}

TEST(PriceLevelTest, AddAndRemoveOrder)
{
  PriceLevel priceLevel(100);
  auto       buyOrder = createOrder(OrderType::BUY, 100, 10, 1);
  priceLevel.addOrder(std::move(buyOrder));

  EXPECT_EQ(priceLevel.getOrders().size(), 1);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

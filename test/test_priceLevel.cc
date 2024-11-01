#include "priceLevel.hh"
#include <gtest/gtest.h>

TEST(PriceLevelTest, PriceLevelCreation)
{
  PriceLevel priceLevel(100);
  EXPECT_EQ(priceLevel.getPrice(), 100);
}

TEST(PriceLevelTest, AddAndRemoveOrder)
{
  PriceLevel priceLevel(100);
  Order      buyOrder(1, OrderType::BUY, 100, 10);
  priceLevel.addOrder(&buyOrder);

  EXPECT_EQ(priceLevel.getOrders().size(), 1);

}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

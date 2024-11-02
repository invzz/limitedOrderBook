#include "order.hh"
#include <gtest/gtest.h>

TEST(OrderTest, OrderCreation)
{
  Order order(OrderType::BUY, 100, 10, 42);
  EXPECT_EQ(order.getType(), OrderType::BUY);
  EXPECT_EQ(order.getPrice(), 100);
  EXPECT_EQ(order.getQuantity(), 10);
  EXPECT_EQ(order.getUserId(), 42);
}

TEST(OrderTest, UpdateQuantity)
{
  Order order(OrderType::SELL, 150, 20, 0);
  order.updateQuantity(-5);
  EXPECT_EQ(order.getQuantity(), 15);
  order.updateQuantity(-20);
  EXPECT_EQ(order.getQuantity(), 0); // Testing negative quantity handling
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
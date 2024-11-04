// buyerBot.cc
#include "buyerBot.hh"
#include <random>
#include <spdlog/spdlog.h>

std::unique_ptr<Order> BuyerBot::createBuyOrder()
{
  std::uniform_int_distribution<int> quantity_dist(1, max_quantity);
  // Buying at prices higher than current market prices to be competitive
  std::uniform_real_distribution<double> price_dist(min_price, max_price);

  int    quantity = quantity_dist(rng);
  double price    = price_dist(rng);

  return std::make_unique<Order>(OrderType::BUY, price, quantity, userId);
}

void BuyerBot::run()
{
  this->rng.seed(std::random_device()());

  // Get the order book from the server
  auto &orderBook  = server->getOrderBook();
  auto &sellOrders = orderBook.getSellOrders(); // Assuming you have a method to get sell orders

  double targetPrice = max_price; // Start with the maximum price the bot is willing to pay

  // Analyze the current sell orders to adjust buying strategy
  if(!sellOrders.empty())
    {
      // Find the lowest sell order price to help determine a competitive price
      auto lowestSellOrder = sellOrders.begin(); // The first entry in the map is the lowest price

      // If there are sell orders, use the lowest price to inform buying strategy
      if(lowestSellOrder != sellOrders.end())
        {
          targetPrice = lowestSellOrder->first; // Target the lowest sell order price
        }
    }

  // Create a buy order at a price slightly above the lowest sell order to increase chances of
  // execution
  if(targetPrice < max_price)
    {
      targetPrice += 0.05; // Set a target price slightly higher to be competitive
    }

  // Create and place the buy order
  auto buyOrder = std::make_unique<Order>(OrderType::BUY, targetPrice, max_quantity, userId);
  server->addOrder(std::move(buyOrder));
}
#include "orderBookServer.hh"
#include "order.hh"
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>
#include <random>

void botFunction(OrderBookServer &server, int userId, int numOrders)
{
  for(int i = 0; i < numOrders; ++i)
    {
      // create a random order
      std::mt19937                           rng(std::random_device{}());
      std::uniform_int_distribution<int>     quantity_dist(1, 100);
      std::uniform_real_distribution<double> price_dist(1.0, 110.0);
      std::uniform_int_distribution<int>     type_dist(0, 1); // 0 for BUY, 1 for SELL

      OrderType type     = (type_dist(rng) == 0) ? OrderType::BUY : OrderType::SELL;
      double    price    = price_dist(rng);
      int       quantity = quantity_dist(rng);

      // Create an order
      auto order = std::make_unique<Order>(type, price, quantity, userId);
      // Add order to the server
      server.addOrder(std::move(order));
      // Simulate delay between orders
      // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
  spdlog::set_level(spdlog::level::info);

  OrderBookServer server;

  // Define number of bots and orders per bot
  int numBots      = 5;
  int ordersPerBot = 1000;

  std::vector<std::thread> bots;
  for(int i = 0; i < numBots; ++i) { bots.emplace_back(botFunction, std::ref(server), i + 1, ordersPerBot); }

  // Wait for all bots to finish
  for(auto &bot : bots) { bot.join(); }

  spdlog::info("All bots have finished placing orders.");

  // server.match(); no needed, match is done real time
  return 0;
}

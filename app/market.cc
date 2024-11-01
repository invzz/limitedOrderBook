#include <vector>
#include <thread>
#include "orderBook.hh" // Include OrderBook header
#include "order.hh"     // Include Order header
#include "bot.hh"       // Include Bot header (if separate)

int main()
{
  OrderBook orderBook;
  int       num_bots          = 5;
  int       orders_per_second = 10;
  int       duration_seconds  = 5;

  std::vector<std::thread> bots;

  for(int i = 0; i < num_bots; ++i)
    {
      bots.emplace_back(
        Bot(orderBook, i, orders_per_second, duration_seconds, i));
    }

  for(auto &bot : bots)
    {
      bot.join(); // Wait for all bots to finish
    }

  spdlog::info("All bots have finished placing orders.");
  return 0;
}

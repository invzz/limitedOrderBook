#include "marketSimulator.hh"
#include "sellerBot.hh"
#include "buyerBot.hh"
#include "marketMakerBot.hh"
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>
#include <random>
#define NUM_TICKS        1000
#define TICK_DURATION_MS 0
int main()
{
  spdlog::set_level(spdlog::level::debug); // Set the log level to debug
  OrderBookService server;

  // Create bots with different strategies
  server.addBot(std::make_unique<SellerBot>(1, "Seller_1", &server, 80.0, 110.0));
  server.addBot(std::make_unique<SellerBot>(2, "Seller_2", &server, 75.0, 130.0));
  server.addBot(std::make_unique<BuyerBot>(3, "Buyer_1", &server));
  server.addBot(std::make_unique<BuyerBot>(4, "Buyer_2", &server));

  server.tick(NUM_TICKS, TICK_DURATION_MS); // Start the market simulation
  server.logMetrics();
  return 0;
}

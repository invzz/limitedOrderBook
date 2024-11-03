#include "orderBookServer.hh"
#include <iostream>
#include <fstream>

void OrderBookServer::addOrder(std::unique_ptr<Order> order)
{
  order->setId(orderBook.getNextOrderId());

  int       id         = order->getId();
  double    price      = order->getPrice();
  int       quantity   = order->getQuantity();
  OrderType type       = order->getType();
  int       userId     = order->getUserId();
  auto      stringType = (type == OrderType::BUY) ? "BUY " : "SELL";

  if(quantity <= 0)
    {
      spdlog::error(
        "[ OrderBook ] :: User {} >> [ {} Order ] < {:^5} > has invalid quantity: {:^5}.", userId,
        stringType, id, quantity);
      return;
    }

  std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access
  orderBook.addOrder(std::move(order));  // Release ownership of the order
  spdlog::debug("[ OrderBook ] :: User {} >> [ {} Order ] < {:^5} > {:^.2f} x {:^4} Added .",
                userId, stringType, id, price, quantity);

  orderBook.match(metricsMap); // Match the orders in the order book
}

void OrderBookServer::addBot(std::unique_ptr<Bot> bot)
{
  int userId         = bot->getUserId();
  metricsMap[userId] = &bot->getMetrics(); // Store pointer to bot's metrics
  bots[userId]       = std::move(bot);     // Move bot into the map with userId as the key
}

void OrderBookServer::tick(int numTicks, int tickDurationMs)
{
  for(int i = 0; i < numTicks; i++)
    {
      for(auto &botPair : bots)
        {
          // botPair is a std::pair<int, std::unique_ptr<Bot>>
          Bot *bot = botPair.second.get(); // Get raw pointer to Bot
          bot->run(); // Each bot can access the order book through getOrderBook()
        }
    }
  std::this_thread::sleep_for(std::chrono::milliseconds(tickDurationMs));
}

void OrderBookServer::logMetrics()
{
  for(auto &pair : metricsMap)
    {
      int      userId  = pair.first;
      Metrics *metrics = pair.second;
      auto     bot     = bots[userId].get();
      spdlog::info("[ Metrics ] :: User {} >> {}", bot->getName(), metrics->toString());

      std::ofstream myfile;

      std::string buyfile  = OUTPUT_DIR + std::to_string(userId) + "_BUY" + ".csv";
      std::string sellfile = OUTPUT_DIR + std::to_string(userId) + "_SELL" + ".csv";

      if(metrics->getSellTrades().size() > 0)
        {
          myfile.open(sellfile, std::ofstream::out | std::ofstream::trunc);
          myfile << metrics->getSellTradesCsv();
          myfile.close();
        }

      if(metrics->getBuyTrades().size() > 0)
        {
          myfile.open(buyfile, std::ofstream::out | std::ofstream::trunc);
          myfile << metrics->getBuyTradesCsv();
          myfile.close();
        }
    }
}

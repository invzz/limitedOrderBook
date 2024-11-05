#include "abstractBot.hh"
#include <random>

class SimpleBot : public Bot
{
  public:
  SimpleBot(const std::string &serverAddress, int userId) : Bot(serverAddress), userId(userId)
  {
    rng.seed(std::random_device()());
  }

  protected:
  void run() override
  {
    // Add logic for the bot's behavior here based on the updated order book
    // Example: analyze the order book and send an order
    analyzeOrderBook();

    // random choose between buy and sell

    std::uniform_int_distribution<int> dist(0, 1);
    OrderType                          t = dist(rng) == 0 ? OrderType::BUY : OrderType::SELL;

    std::unique_ptr<Order> order = std::make_unique<Order>(t, 100.0, 10, userId);
    sendOrder(order->toJson());
  }

  private:
  std::mt19937 rng;
  int          userId;

  void analyzeOrderBook() { spdlog::info("Analyzing order book..."); }
};
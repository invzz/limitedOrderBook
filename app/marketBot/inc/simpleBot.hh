#include "abstractBot.hh"
#include <random>

class SimpleBot : public Bot
{
  public:
  SimpleBot(const std::string &serverAddress, int userId, OrderType t)
      : Bot(serverAddress, userId, t)
  {
    rng.seed(std::random_device()());
  }

  protected:
  void run() override
  {
    analyzeOrderBook();

    // generate a random price between 90 and 110
    std::uniform_real_distribution<double> priceDist(90.0, 110.0);
    double                                 price = priceDist(rng);

    // Generate a random order quantity between 1 and 10
    std::uniform_int_distribution<int> quantityDist(1, 10);
    int                                quantity = quantityDist(rng);

    std::unique_ptr<Order> order = std::make_unique<Order>(getOrderType(), price, quantity, getUserId());
    sendOrder(order->toJson());
  }

  private:
  std::mt19937 rng;

  void analyzeOrderBook() { return; }
};
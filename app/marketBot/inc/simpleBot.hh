#include "abstractBot.hh"

#include <random>

class SimpleBot : public Bot
{
  public:
  SimpleBot(const std::string &serverAddress, std::string userId, OrderType t) : Bot(serverAddress, userId), type(t) { rng.seed(std::random_device()()); }

  protected:
  void run() override
  {
    // generate a random price between 90 and 110
    std::uniform_real_distribution<double> priceDist(90.0, 110.0);
    double                                 price = priceDist(rng);

    // Generate a random order quantity between 1 and 10
    std::uniform_int_distribution<int> quantityDist(1, 20);
    int                                quantity = quantityDist(rng);

    std::unique_ptr<Order> order = std::make_unique<Order>(type, price, quantity, getUserId());
    putOrder(order->toJson());
  }

  private:
  std::mt19937 rng;
  OrderType    type;
};
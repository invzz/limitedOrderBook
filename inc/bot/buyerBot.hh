// buyerBot.hh
#ifndef BUYER_BOT_HH
#define BUYER_BOT_HH

#include "orderBookServer.hh"
#include "order.hh"
#include <string>
#include <map>

class BuyerBot : public Bot
{
  public:
  BuyerBot(int userId, const std::string &name, OrderBookServer *server) : Bot(userId, name, server)
  {}

  void run() override;

  private:
  double max_price    = 110.0;
  double min_price    = 1;  // Maximum price the buyer is willing to pay
  int    max_quantity = 20; // Maximum quantity the buyer is willing to purchase

  std::unique_ptr<Order> createBuyOrder();
};

#endif // BUYER_BOT_HH

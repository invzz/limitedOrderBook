// sellerBot.hh
#ifndef SELLER_BOT_HH
#define SELLER_BOT_HH

#include "bot.hh"
#include <random>

class SellerBot : public Bot
{
  public:
  SellerBot(int userId, const std::string &name, OrderBookServer *server, double min, double max)
      : Bot(userId, name, server), min_price(min), max_price(max)
  {}

  void run() override;

  private:
  double min_price;
  double max_price;
  int    max_quantity = 20;
};

#endif // SELLER_BOT_HH

// marketMakerBot.hh
#ifndef MARKET_MAKER_BOT_HH
#define MARKET_MAKER_BOT_HH

#include "bot.hh"

class MarketMakerBot : public Bot
{
  public:
  MarketMakerBot(int userId, const std::string &name, MarketSimulator *server)
      : Bot(userId, name, server)
  {}

  void run() override; // Override run for MarketMakerBot behavior
};

#endif // MARKET_MAKER_BOT_HH

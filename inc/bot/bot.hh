// bot.hh
#ifndef BOT_HH
#define BOT_HH

#include <memory>
#include <string>
#include <random>
#include "order.hh"
#include "metrics.hh"

// Forward declaration to avoid circular dependency
class OrderBookServer;

class Bot
{
  public:
  virtual ~Bot()     = default;
  virtual void run() = 0; // Pure virtual to enforce implementation in derived classes

  Bot(int userId, const std::string name, OrderBookServer *server)
      : userId(userId), name(name), server(server)
  {
    this->rng.seed(std::random_device()());
  };

  std::string getName() const { return name; }
  int         getUserId() const { return userId; }
  Metrics    &getMetrics() { return metrics; }

  protected:
  std::mt19937     rng;
  int              userId;
  std::string      name;
  OrderBookServer *server; 
  Metrics          metrics;
};

#endif // BOT_HH

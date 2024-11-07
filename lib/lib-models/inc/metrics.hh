// metrics.hh
#ifndef METRICS_HH
#define METRICS_HH

#include <vector>
#include <numeric>
#include <mutex>
#include <sstream>
#include "trade.hh"

#define SEPARATOR " - "

class Metrics
{
  public:
  Metrics() : totalProfit(0.0), position(0) {}

  void addBuyTrade(const std::shared_ptr<Trade> &trade)
  {
    std::scoped_lock<std::shared_mutex> lock(buyTradesMtx);
    buyTrades.push_back(trade);
    totalProfit -= trade->getPrice() * trade->getQuantity();
    position += trade->getQuantity();
  }

  void addSellTrade(const std::shared_ptr<Trade> &trade)
  {
    std::scoped_lock<std::shared_mutex> lock(sellTradesMtx);
    sellTrades.push_back(trade);
    totalProfit += trade->getPrice() * trade->getQuantity();
    position -= trade->getQuantity();
  }

  std::string toString()
  {
    std::stringstream ss;
    ss << "buy    :: " << buyTrades.size() << SEPARATOR;
    ss << "sell   :: " << sellTrades.size() << SEPARATOR;
    ss << "position :: " << position << SEPARATOR;
    ss << "profit :: " << totalProfit;
    return ss.str();
  }

  const std::vector<std::shared_ptr<Trade>> &getBuyTrades() const { return buyTrades; }
  const std::vector<std::shared_ptr<Trade>> &getSellTrades() const { return sellTrades; }
  double                                     getProfit() { return totalProfit; }
  int                                        getPosition() { return position; }
  
  void                                       updatePosition(int quantity) { position += quantity; }

  bool isWithinLimit(int additionalUnits, int positionLimit) const
  {
    return (position + additionalUnits <= positionLimit) && (position + additionalUnits >= -positionLimit);
  }

  private:
  mutable std::shared_mutex           buyTradesMtx;
  mutable std::shared_mutex           sellTradesMtx;
  double                              totalProfit;
  int                                 position;
  std::vector<std::shared_ptr<Trade>> buyTrades;
  std::vector<std::shared_ptr<Trade>> sellTrades;
};

#endif // METRICS_HH

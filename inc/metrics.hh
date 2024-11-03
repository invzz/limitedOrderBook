// metrics.hh
#ifndef METRICS_HH
#define METRICS_HH

#include <vector>
#include "trade.hh"

class Metrics
{
  public:
  Metrics() : totalTrades(0), totalProfit(0.0) {}

  void addBuyTrade(const Trade &trade)
  {
    buyTrades.push_back(trade);
    totalTrades++;
    totalProfit -= trade.getPrice() * trade.getQuantity(); // Assuming cost is negative for buys
  }

  void addSellTrade(const Trade &trade)
  {
    sellTrades.push_back(trade);
    totalTrades++;
    totalProfit += trade.getPrice() * trade.getQuantity(); // Assuming profit is positive for sells
  }

  std::string toString() const
  {
    return "Total Trades: " + std::to_string(totalTrades) +
           ", Total Profit: " + std::to_string(totalProfit);
  }

  std::string getBuyTradesCsv()
  {
    std::string csv = "BuyerID, SellerID, Price, Quantity\n";
    for(const auto &trade : buyTrades)
      {
        csv += std::to_string(trade.getBuyerId()) + ", " + std::to_string(trade.getSellerId()) +
               ", " + std::to_string(trade.getPrice()) + ", " +
               std::to_string(trade.getQuantity()) + "\n";
      }

    return csv;
  }

  std::string getSellTradesCsv()
  {
    std::string csv = "BuyerID, SellerID, Price, Quantity\n";
    for(const auto &trade : sellTrades)
      {
        csv += std::to_string(trade.getBuyerId()) + ", " + std::to_string(trade.getSellerId()) +
               ", " + std::to_string(trade.getPrice()) + ", " +
               std::to_string(trade.getQuantity()) + "\n";
      }

    return csv;
  }

  int                       getTotalTrades() const { return totalTrades; }
  double                    getTotalProfit() const { return totalProfit; }
  const std::vector<Trade> &getBuyTrades() const { return buyTrades; }
  const std::vector<Trade> &getSellTrades() const { return sellTrades; }

  private:
  int                totalTrades;
  double             totalProfit;
  std::vector<Trade> buyTrades;
  std::vector<Trade> sellTrades;
};

#endif // METRICS_HH

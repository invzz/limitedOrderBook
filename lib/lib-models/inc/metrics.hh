// metrics.hh
#ifndef METRICS_HH
#define METRICS_HH

#include <vector>
#include "trade.hh"

#define CSV_SEPARATOR ", "

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
    std::string csv = "time, BuyerID, SellerID, Price, Quantity\n";
    for(const auto &trade : buyTrades)
      {
        auto tick      = std::to_string(trade.getTick());
        auto seller_id = std::to_string(trade.getSellerId());
        auto buyer_id  = std::to_string(trade.getBuyerId());
        auto quantity  = std::to_string(trade.getQuantity());
        auto price     = std::to_string(trade.getPrice());

        csv += tick;
        csv += CSV_SEPARATOR;
        csv += buyer_id;
        csv += CSV_SEPARATOR;
        csv += seller_id;
        csv += CSV_SEPARATOR;
        csv += price;
        csv += CSV_SEPARATOR;
        csv += quantity;
        csv += "\n";
      }

    return csv;
  }

  std::string getSellTradesCsv()
  {
    std::string csv = "time, BuyerID, SellerID, Price, Quantity\n";

    for(const auto &trade : sellTrades)
      {
        auto tick      = std::to_string(trade.getTick());
        auto seller_id = std::to_string(trade.getSellerId());
        auto buyer_id  = std::to_string(trade.getBuyerId());
        auto quantity  = std::to_string(trade.getQuantity());
        auto price     = std::to_string(trade.getPrice());

        csv += tick;
        csv += CSV_SEPARATOR;
        csv += buyer_id;
        csv += CSV_SEPARATOR;
        csv += seller_id;
        csv += CSV_SEPARATOR;
        csv += price;
        csv += CSV_SEPARATOR;
        csv += quantity;
        csv += "\n";
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

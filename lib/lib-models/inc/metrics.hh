// metrics.hh
#ifndef METRICS_HH
#define METRICS_HH

#include <vector>
#include <numeric>
#include <mutex>
#include "trade.hh"

#define CSV_SEPARATOR ", "

class Metrics
{
  public:
  Metrics() : totalProfit(0.0) {}

  void addBuyTrade(const std::shared_ptr<Trade> &trade) { 
    std::lock_guard<std::mutex> lock(mtx);
    buyTrades.push_back(trade); }

  void addSellTrade(const std::shared_ptr<Trade> &trade) {
    std::lock_guard<std::mutex> lock(mtx);
    sellTrades.push_back(trade); }

  double calcProfit()
  {
    for(const auto &trade : buyTrades) { totalProfit -= trade->getPrice() * trade->getQuantity(); }
    for(const auto &trade : sellTrades) { totalProfit += trade->getPrice() * trade->getQuantity(); }
    return totalProfit;
  }

  std::string toString()
  {
    auto totalProfit = calcProfit();
    return "[ BUY  Trades ] :: " + std::to_string(buyTrades.size()) + " " +
           "[ SELL Trades ] :: " + std::to_string(sellTrades.size()) + " " +
           "[ Profit ]      :: " + std::to_string(totalProfit);
  }

  std::string getBuyTradesCsv()
  {
    std::string csv = "time, BuyerID, SellerID, Price, Quantity\n";
    for(const auto &trade : buyTrades)
      {
        auto tick      = std::to_string(trade->getTick());
        auto seller_id = std::to_string(trade->getSellerId());
        auto buyer_id  = std::to_string(trade->getBuyerId());
        auto quantity  = std::to_string(trade->getQuantity());
        auto price     = std::to_string(trade->getPrice());
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
        auto tick      = std::to_string(trade->getTick());
        auto seller_id = std::to_string(trade->getSellerId());
        auto buyer_id  = std::to_string(trade->getBuyerId());
        auto quantity  = std::to_string(trade->getQuantity());
        auto price     = std::to_string(trade->getPrice());
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

  double                                     getTotalProfit() const { return totalProfit; }
  const std::vector<std::shared_ptr<Trade>> &getBuyTrades() const { return buyTrades; }
  const std::vector<std::shared_ptr<Trade>> &getSellTrades() const { return sellTrades; }
  mutable std::mutex                         mtx; // Mutex to protect buyTrades and sellTrades

  private:
  double                              totalProfit;
  std::vector<std::shared_ptr<Trade>> buyTrades;
  std::vector<std::shared_ptr<Trade>> sellTrades;
};

#endif // METRICS_HH

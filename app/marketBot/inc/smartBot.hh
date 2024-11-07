#include "abstractBot.hh"
#include <random>

class SmartBot : public Bot
{
  public:
  SmartBot(const std::string &serverAddress, std::string name, int userId) : Bot(serverAddress, name, userId) { rng.seed(std::random_device()()); }

  protected:
  void run() override
  {
    analyzeOrderBook();
    // pop all the orders from the order vector and send them
    for(auto &order : orders) { sendOrder(order->toJson()); }
    orders.clear();
  }

  private:
  std::mt19937 rng;
  double       avgPrice       = 0.0;
  int          targetQuantity = 0;

  std::vector<std::unique_ptr<Order>> orders;

  // Function to retrieve total quantity of orders at the best ask or bid, excluding own orders
  int getMarketDepth(const std::vector<std::shared_ptr<Order>> &orders)
  {
    int totalQuantity = 0;
    for(auto &order : orders)
      {
        if(order->getUserId() != getUserId())
          { // Exclude own orders
            totalQuantity += order->getQuantity();
          }
      }
    return totalQuantity;
  }

  // Function to calculate the average price between the best ask and best bid
  double calculateAvgPrice(double bestBidPrice, double bestAskPrice) { return (bestBidPrice + bestAskPrice) / 2; }

  // Function to calculate the spread between the best ask and best bid
  double calculateSpread(double bestBidPrice, double bestAskPrice) { return bestAskPrice - bestBidPrice; }

  // Function to determine if a buy order should be placed
  bool shouldPlaceBuyOrder(double bestBidPrice, double avgPrice, double spread)
  {
    return bestBidPrice > avgPrice; // Example condition: Buy if best bid is greater than avgPrice
  }

  // Function to determine if a sell order should be placed
  bool shouldPlaceSellOrder(double bestAskPrice, double avgPrice, double spread)
  {
    return bestAskPrice < avgPrice; // Example condition: Sell if best ask is less than avgPrice
  }

  // Helper function to place an order (either buy or sell)
  void placeOrder(OrderType type, double price, int quantity) { orders.push_back(std::make_unique<Order>(type, price, quantity, getUserId())); }

  // The main analysis function that uses the smaller helper functions
  void analyzeOrderBook()
  {
    // Retrieve best ask and bid, excluding own orders
    auto bestAsk = orderBook->getBestAsk();
    auto bestBid = orderBook->getBestBid();

    // If there are no orders at the best ask or best bid, return
    if(bestAsk.empty() || bestBid.empty()) { return; }

    // Get the best ask and bid prices
    double bestAskPrice = bestAsk.front()->getPrice();
    double bestBidPrice = bestBid.front()->getPrice();

    // Calculate the spread and the average price
    double spread = calculateSpread(bestBidPrice, bestAskPrice);
    avgPrice      = calculateAvgPrice(bestBidPrice, bestAskPrice);

    // Calculate market depth at the best ask and best bid, excluding own orders
    int totalQuantityAtAsk = getMarketDepth(bestAsk);
    int totalQuantityAtBid = getMarketDepth(bestBid);

    // Adjust aggressiveness factor based on your strategy
    double aggressivenessFactor = 0.2; // Adjust this based on desired behavior

    // If the spread is wide, place aggressive orders
    if(spread > 0)
      {
        if(shouldPlaceBuyOrder(bestBidPrice, avgPrice, spread))
          {
            double targetBuyPrice = bestBidPrice + aggressivenessFactor * spread; // Buy slightly above the best bid
            int    targetQuantity = totalQuantityAtAsk > 1 ? totalQuantityAtAsk / 2 : 1;
            placeOrder(OrderType::BUY, targetBuyPrice, targetQuantity);
          }

        if(shouldPlaceSellOrder(bestAskPrice, avgPrice, spread))
          {
            double targetSellPrice = bestAskPrice - aggressivenessFactor * spread; // Sell slightly below the best ask
            int    targetQuantity  = totalQuantityAtBid > 1 ? totalQuantityAtBid / 2 : 1;
            placeOrder(OrderType::SELL, targetSellPrice, targetQuantity);
          }
      }
    else
      {
        // If the spread is narrow, place passive orders
        if(shouldPlaceBuyOrder(bestBidPrice, avgPrice, spread))
          {
            double targetBuyPrice = bestBidPrice; // Buy at the best bid
            targetQuantity        = totalQuantityAtAsk > 1 ? totalQuantityAtAsk / 2 : 1;
            placeOrder(OrderType::BUY, targetBuyPrice, targetQuantity);
          }

        if(shouldPlaceSellOrder(bestAskPrice, avgPrice, spread))
          {
            double targetSellPrice = bestAskPrice; // Sell at the best ask
            int    targetQuantity  = totalQuantityAtBid > 1 ? totalQuantityAtBid / 2 : 1;
            placeOrder(OrderType::SELL, targetSellPrice, targetQuantity);
          }
      }
  }

  // Helper to generate random quantity
  int generateRandomQuantity()
  {
    std::uniform_int_distribution<int> quantityDist(1, 10); // Arbitrary range for quantity
    return quantityDist(rng);
  }
};
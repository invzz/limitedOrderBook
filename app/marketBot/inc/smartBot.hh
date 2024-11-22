#include "MarketClient.hh"

class SmartBot : public MarketClient
{
    public:
    SmartBot( std::string userId) : MarketClient( userId) { rng.seed(std::random_device()()); }

    protected:
    void run() override
    {
        if(!orderBookService) { return; }
        auto bestAsk = orderBookService->getBestAsk();
        auto bestBid = orderBookService->getBestBid();

        if(bestAsk.empty() || bestBid.empty()) { return; }

        double bestAskPrice = orderBookService->getBestAskPrice();
        double bestBidPrice = orderBookService->getBestBidPrice();

        double spread = calculateSpread(bestBidPrice, bestAskPrice);
        avgPrice      = calculateAvgPrice(bestBidPrice, bestAskPrice);

        int totalQuantityAtAsk = getMarketDepth(bestAsk);
        int totalQuantityAtBid = getMarketDepth(bestBid);

        double aggressivenessFactor = 0.2;

        // If spread is wide, place aggressive orders
        if(spread > 0)
            {
                if(shouldPlaceBuyOrder(bestBidPrice, avgPrice, spread))
                    {
                        double targetBuyPrice = bestBidPrice + aggressivenessFactor * spread;
                        int    targetQuantity = totalQuantityAtAsk > 1 ? totalQuantityAtAsk / 2 : 1;
                        placeOrder(OrderType::BUY, targetBuyPrice, targetQuantity);
                    }

                if(shouldPlaceSellOrder(bestAskPrice, avgPrice, spread))
                    {
                        double targetSellPrice = bestAskPrice - aggressivenessFactor * spread;
                        int    targetQuantity  = totalQuantityAtBid > 1 ? totalQuantityAtBid / 2 : 1;
                        placeOrder(OrderType::SELL, targetSellPrice, targetQuantity);
                    }
            }
        else
            {
                // If the spread is narrow, place passive orders
                if(shouldPlaceBuyOrder(bestBidPrice, avgPrice, spread))
                    {
                        double targetBuyPrice = bestBidPrice;
                        targetQuantity        = totalQuantityAtAsk > 1 ? totalQuantityAtAsk / 2 : 1;
                        placeOrder(OrderType::BUY, targetBuyPrice, targetQuantity);
                    }

                if(shouldPlaceSellOrder(bestAskPrice, avgPrice, spread))
                    {
                        double targetSellPrice = bestAskPrice;
                        int    targetQuantity  = totalQuantityAtBid > 1 ? totalQuantityAtBid / 2 : 1;
                        placeOrder(OrderType::SELL, targetSellPrice, targetQuantity);
                    }
            }

        for(auto &order : orders) { putOrder(order->getAsJson()); }
    }

    private:
    std::mt19937                        rng;
    double                              avgPrice       = 0.0;
    int                                 targetQuantity = 0;
    std::vector<std::unique_ptr<Order>> orders;

    // Function to retrieve total quantity of orders at the best ask or bid, excluding own orders
    int getMarketDepth(const std::vector<std::shared_ptr<Order>> &orders)
    {
        int totalQuantity = 0;
        for(auto &order : orders)
            {
                if(order->getUserId() != getUserId()) // Exclude own orders
                    {
                        totalQuantity += order->getQuantity();
                    }
            }
        return totalQuantity;
    }

    double calculateAvgPrice(double bestBidPrice, double bestAskPrice) { return (bestBidPrice + bestAskPrice) / 2; }

    double calculateSpread(double bestBidPrice, double bestAskPrice) { return bestAskPrice - bestBidPrice; }

    bool shouldPlaceBuyOrder(double bestBidPrice, double avgPrice, double spread)
    {
        return bestBidPrice > avgPrice; // Buy if best bid is greater than avgPrice
    }

    bool shouldPlaceSellOrder(double bestAskPrice, double avgPrice, double spread)
    {
        return bestAskPrice < avgPrice; // Sell if best ask is less than avgPrice
    }

    void placeOrder(OrderType type, double price, int quantity) { orders.push_back(std::make_unique<Order>(type, price, quantity, getUserId())); }
};
#include "MultiMarketClient.hh"

#include <random>

using namespace market;

class MultiSimpleBot : public MultiMarketClient
{
    public:
    MultiSimpleBot(std::string userId, OrderType t) : MultiMarketClient(userId), type(t) { rng.seed(std::random_device()()); }

    protected:
    void run() override
    {
        // generate a random price between 90 and 110
        std::uniform_real_distribution<double> priceDist(95.0, 100.0);
        double                                 price = priceDist(rng);

        double rounded = std::round(10. * price) / 10.;

        // Generate a random order quantity between 1 and 10
        std::uniform_int_distribution<int> quantityDist(1, 20);
        int                                quantity = quantityDist(rng);

        std::unique_ptr<Order> order = std::make_unique<Order>(type, rounded, quantity, getUserId());


        std::vector<std::string> products;

        products.push_back("BTC");
        products.push_back("ETH");
        products.push_back("LTC");
        products.push_back("XRP");


        putOrder("BTC", order->getAsJson());
        putOrder("ETH", order->getAsJson());
        putOrder("LTC", order->getAsJson());
        putOrder("XRP", order->getAsJson());
        
    }

    private:
    std::mt19937 rng;
    OrderType    type;
};
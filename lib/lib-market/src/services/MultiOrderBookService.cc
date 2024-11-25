#include "MultiOrderBookService.hh"
namespace market
{

    std::shared_ptr<OrderBookService> MultiOrderBookService::getOrCreate(const std::string &productId)
    {
        if(productOrderBooks_.find(productId) == productOrderBooks_.end()) { productOrderBooks_[productId] = std::make_shared<OrderBookService>(productId); }
        return productOrderBooks_[productId];
    }

    nlohmann::json MultiOrderBookService::getAsJson()
    {
        nlohmann::json result;
        for(const auto &[p, s] : productOrderBooks_) { result[p] = s->getAsJson(); }
        return result;
    }

    double MultiOrderBookService::getAvgPrice(const std::string &productId)
    {
        auto service = getOrCreate(productId);
        return service->getAvgPrice();
    }

    size_t MultiOrderBookService::getTotalOrdersCount(const std::string &productId)
    {
        auto service = getOrCreate(productId);
        return service->getTotalOrdersCount();
    }

    void MultiOrderBookService::updateOrderBook(const std::string &productId, const nlohmann::json &orderBookData, bool doClean)
    {
        auto service = getOrCreate(productId);
        service->updateOrderBook(orderBookData, doClean);
    }

    void MultiOrderBookService::updateOrderBook(nlohmann::json &orderBookData, bool doClean)
    {
        for(const auto &product : orderBookData.items()) { updateOrderBook(product.key(), product.value(), doClean); }
    }

    void MultiOrderBookService::clear()
    {
        for(auto &pair : productOrderBooks_) { pair.second->clear(); }
    }

    std::vector<std::string> MultiOrderBookService::getProducts() const
    {
        std::vector<std::string> products;
        for(const auto &pair : productOrderBooks_) { products.push_back(pair.first); }
        return products;
    }

} // namespace market
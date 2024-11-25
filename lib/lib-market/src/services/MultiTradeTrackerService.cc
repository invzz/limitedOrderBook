#include "MultiTradeTrackerService.hh"
namespace market
{

    std::shared_ptr<TradeTrackerService> MultiTradeTrackerService::getOrCreate(const std::string &productId)
    {
        if(productOrderBooks_.find(productId) == productOrderBooks_.end()) { productOrderBooks_[productId] = std::make_shared<TradeTrackerService>(); }
        return productOrderBooks_[productId];
    }

    nlohmann::json MultiTradeTrackerService::getAsJson()
    {
        nlohmann::json result;
        for(const auto &[p, s] : productOrderBooks_) { result[p] = s->getAsJson(); }
        return result;
    }
} // namespace market
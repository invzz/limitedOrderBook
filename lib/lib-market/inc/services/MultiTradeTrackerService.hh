#pragma once
#include "tradeTrackerService.hh"

namespace market
{
    class MultiTradeTrackerService
    {
        public:
        MultiTradeTrackerService() {}

        std::shared_ptr<TradeTrackerService> getOrCreate(const std::string &product);
        nlohmann::json                       getAsJson();

        private:
        std::unordered_map<std::string, std::shared_ptr<TradeTrackerService>> productOrderBooks_;
    };
} // namespace market

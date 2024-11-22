#ifndef TRADE_TRACKER_SERVICE_HH
#define TRADE_TRACKER_SERVICE_HH
#include <memory>
#include <nlohmann/json.hpp>
#include <atomic>
#include "tradeRepository.hh"
#include "tradeService.hh"
#include "trade.hh"
#include "spdlog/spdlog.h"
namespace market
{
    class TradeTrackerService
    {
        public:
        TradeTrackerService() { spdlog::info("[TradeTrackerService] Creating TradeTrackerService instance"); }
        void addBuyById(const std::string &userId, const std::shared_ptr<Trade> &trade)
        {
            auto it = tradeServices_.find(userId);
            if(it != tradeServices_.end()) { it->second->buy(trade); }
            else
                {
                    auto service           = std::make_shared<TradeService>();
                    tradeServices_[userId] = service;
                    service->buy(trade);
                }
        }

        void addSellById(const std::string &userId, const std::shared_ptr<Trade> &trade)
        {
            auto it = tradeServices_.find(userId);
            if(it != tradeServices_.end()) { it->second->sell(trade); }
            else
                {
                    auto service           = std::make_shared<TradeService>();
                    tradeServices_[userId] = service;
                    service->sell(trade);
                }
        }

        std::vector<std::string> getIds() const
        {
            auto ids = std::vector<std::string>();
            for(const auto &[userId, service] : tradeServices_) { ids.push_back(userId); }
            return ids;
        }

        std::shared_ptr<TradeService> getTradeService(const std::string &userId) const
        {
            auto it = tradeServices_.find(userId);
            if(it != tradeServices_.end()) { return it->second; }
            return nullptr;
        }

        nlohmann::json getAsJson(bool serializeTrades = true) const
        {
            nlohmann::json j;
            for(const auto &[userId, service] : tradeServices_) { j[userId] = service->getAsJson(serializeTrades); }
            return j;
        }

        nlohmann::json getAsJson(std::string &id) const { return getTradeService(id)->getAsJson(); }

        std::vector<std::shared_ptr<Trade>> getBuyTrades(const std::string &userId) const
        {
            auto it = tradeServices_.find(userId);
            if(it != tradeServices_.end()) { return it->second->getBuyTrades(); }
            return {};
        }

        std::vector<std::shared_ptr<Trade>> getSellTrades(const std::string &userId) const
        {
            auto it = tradeServices_.find(userId);
            if(it != tradeServices_.end()) { return it->second->getSellTrades(); }
            return {};
        }

        private:
        std::unordered_map<std::string, std::shared_ptr<TradeService>> tradeServices_;
    };
} // namespace market
#endif // TRADE_TRACKER_SERVICE_HH
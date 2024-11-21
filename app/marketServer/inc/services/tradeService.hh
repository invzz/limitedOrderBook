#ifndef TRADESERVICE_HH
#define TRADESERVICE_HH
#include <memory>
#include <nlohmann/json.hpp>
#include <atomic>
#include "tradeRepository.hh"
#include "trade.hh"
#include "spdlog/spdlog.h"

class TradeService
{
    public:
    TradeService()
    {
        spdlog::debug("[TradeService] Creating TradeService instance");
        buyTrades_  = std::make_shared<TradeRepository>();
        sellTrades_ = std::make_shared<TradeRepository>();
        position_   = 0;
        profit_     = 0.0;
        next_id_    = 0;
    }

    int generateId() { return next_id_++; }

    void buy(const std::shared_ptr<Trade> &trade)
    {
        auto id = generateId();
        trade->setId(id);
        buyTrades_->add(generateId(), trade);
        double current_profit = profit_.load();
        profit_.store(current_profit - trade->getPrice());
        position_ += trade->getQuantity();
    }

    void sell(const std::shared_ptr<Trade> &trade)
    {
        auto id = generateId();
        trade->setId(id);
        sellTrades_->add(generateId(), trade);
        double current_profit = profit_.load();
        profit_.store(current_profit + trade->getPrice());
        position_ -= trade->getQuantity();
    }

    void liquidate(int tick, double money, std::string userId, std::string liquidator = "liquidator")
    {
        if(position_ > 0)
            {
                auto trade = std::make_shared<Trade>(tick, "liquidator", userId, money, position_);
                sell(trade);
            }
        else if(position_ < 0)
            {
                auto trade = std::make_shared<Trade>(tick, userId, "liquidator", money, position_);
                buy(trade);
            }
    }

    std::vector<std::shared_ptr<Trade>> getBuyTrades() const { return buyTrades_->getAll(); }

    std::vector<std::shared_ptr<Trade>> getSellTrades() const { return sellTrades_->getAll(); }

    nlohmann::json getAsJson(bool serializeTrades = true) const
    {
        nlohmann::json j;
        if(serializeTrades)
            {
                if(buyTrades_->getAll().size() > 0) { j["buyTrades"] = buyTrades_->getAsJson(); }
                if(sellTrades_->getAll().size() > 0) { j["sellTrades"] = sellTrades_->getAsJson(); }
            }
        j["profit"]   = getProfit();
        j["position"] = getPosition();
        return j;
    }

    int    getPosition() const { return position_; }
    double getProfit() const { return profit_.load(); }
    double computeProfit() const
    {
        double profit = 0.0;
        for(const auto &trade : buyTrades_->getAll()) { profit -= trade->getPrice(); }
        for(const auto &trade : sellTrades_->getAll()) { profit += trade->getPrice(); }
        return profit;
    }

    private:
    std::atomic<int>                 position_;
    std::atomic<double>              profit_;
    std::shared_ptr<TradeRepository> buyTrades_;
    std::shared_ptr<TradeRepository> sellTrades_;
    std::atomic<int>                 next_id_;
};
#endif
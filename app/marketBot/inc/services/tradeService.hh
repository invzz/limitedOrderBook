#pragma once
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
        spdlog::info("[TradeService] Creating TradeService instance");
        buyTrades_  = std::make_shared<TradeRepository>();
        sellTrades_ = std::make_shared<TradeRepository>();
        position_   = 0;
        profit_     = 0.0;
        next_id_    = 0;
    }

    int generateId() { return next_id_++; }

    void buy(const std::shared_ptr<Trade> &trade)
    {
        buyTrades_->add(generateId(), trade);
        double current_profit = profit_.load();
        profit_.store(current_profit - trade->getPrice());
        position_ += trade->getQuantity();
    }

    void sell(const std::shared_ptr<Trade> &trade)
    {
        sellTrades_->add(generateId(), trade);
        double current_profit = profit_.load();
        profit_.store(current_profit + trade->getPrice());
        position_ -= trade->getQuantity();
    }

    std::vector<std::shared_ptr<Trade>> getBuyTrades() const { return buyTrades_->getAll(); }

    std::vector<std::shared_ptr<Trade>> getSellTrades() const { return sellTrades_->getAll(); }

    nlohmann::json getAsJson() const
    {
        nlohmann::json j;
        if(buyTrades_->getAll().size() > 0) { j["buyTrades"] = buyTrades_->getAsJson(); }
        if(sellTrades_->getAll().size() > 0) { j["sellTrades"] = sellTrades_->getAsJson(); }
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

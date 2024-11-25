#pragma once
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "orderBookService.hh"
#include "command.hh"
#include "tradeTrackerService.hh"
#include <spdlog/spdlog.h>

#define TRADE_FORMAT "[ {} ] Trade: {} ==> {} [ {:4} @ {:2f} ]"
namespace market
{
    template <typename T> class MarketController
    {
        public:
        MarketController(std::shared_ptr<OrderBookService> orderBookService, std::shared_ptr<TradeTrackerService> tradeTrackerService, std::shared_ptr<T> server);

        void GetMetrics(const std::string &userId);
        void PutOrder(const nlohmann::json &body);
        void match(int tick);
        void Stop();

        private:
        std::shared_ptr<OrderBookService>    orderBookService_;
        std::shared_ptr<TradeTrackerService> tradeTrackerService_;
        std::shared_ptr<T>                   server_;
    };

    template <typename T>
    MarketController<T>::MarketController(std::shared_ptr<OrderBookService> orderBookService, std::shared_ptr<TradeTrackerService> tradeTrackerService, std::shared_ptr<T> server)
        : orderBookService_(orderBookService), tradeTrackerService_(tradeTrackerService), server_(server)
    {}

    template <typename T> void MarketController<T>::GetMetrics(const std::string &userId)
    {
        if(userId.empty()) { return; }
        if(tradeTrackerService_->getTradeService(userId) == nullptr) { return; }

        auto response = tradeTrackerService_->getTradeService(userId)->getAsJson().dump();
        server_->sendMessage(userId, response);
    }

    template <typename T> void MarketController<T>::PutOrder(const nlohmann::json &body)
    {
        auto order = Order::fromJson(body["order"]);
        spdlog::debug("[ {} ] :: {}", market::yellow(__func__), market::blue(order->getAsJson().dump()));
        if(order->getType() == OrderType::BUY) { orderBookService_->addBid(order); }
        else if(order->getType() == OrderType::SELL) { orderBookService_->addAsk(order); }
    }

    template <typename T> void MarketController<T>::match(int tick)
    {
        auto trades = orderBookService_->match(tick);
        for(auto &trade : trades)
            {
                spdlog::info("[ {} ] [ {} ] :: " TRADE_FORMAT, market::yellow(__func__), market::green(orderBookService_->getPoduct()), tick, trade->getSellerId(), trade->getBuyerId(), trade->getQuantity(), trade->getPrice());
                tradeTrackerService_->addBuyById(trade->getBuyerId(), trade);
                tradeTrackerService_->addSellById(trade->getSellerId(), trade);
            }
    }

    template <typename T> void MarketController<T>::Stop() { server_->stop(); }
} // namespace market
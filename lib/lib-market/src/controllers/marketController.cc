#include "marketServer.hh"
#include "marketController.hh"
#include <spdlog/spdlog.h>

namespace market
{

    MarketController::MarketController(std::shared_ptr<OrderBookService> orderBookService, std::shared_ptr<TradeTrackerService> tradeTrackerService,
                                       std::shared_ptr<MarketServer> server)
    {
        orderBookService_    = orderBookService;
        tradeTrackerService_ = tradeTrackerService;
        server_              = server;
    }
    void MarketController::GetMetrics(const std::string &userId)
    {
        if(userId.empty()) { return; }
        if(tradeTrackerService_->getTradeService(userId) == nullptr) { return; }

        auto response = tradeTrackerService_->getTradeService(userId)->getAsJson().dump();
        server_->sendMessage(userId, response);
    }

    void MarketController::PutOrder(const std::string &body)
    {
        nlohmann::json orderJson = nlohmann::json::parse(body);
        auto           order     = Order::fromJson(orderJson);

        spdlog::debug("[ {} ] :: {}", __func__, order->getAsJson().dump());

        if(order->getType() == OrderType::BUY) { orderBookService_->addBid(order); }
        else if(order->getType() == OrderType::SELL) { orderBookService_->addAsk(order); }
    }

    void MarketController::match(int tick)
    {
        auto trades = orderBookService_->match(tick);
        for(auto &trade : trades)
            {
                spdlog::info("[ {} ] :: " TRADE_FORMAT, __func__, tick, trade->getSellerId(), trade->getBuyerId(), trade->getQuantity(), trade->getPrice());
                tradeTrackerService_->addBuyById(trade->getBuyerId(), trade);
                tradeTrackerService_->addSellById(trade->getSellerId(), trade);
            }
    }

    void MarketController::Stop() { server_->stop(); }
} // namespace market
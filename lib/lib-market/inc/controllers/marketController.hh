#pragma once
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "orderBookService.hh"
#include "command.hh"
#include "tradeTrackerService.hh"
#include <spdlog/spdlog.h>

// Forward declaration of MarketServer
class MarketServer;

#define TRADE_FORMAT "[ {} ] Trade: {} ==> {} [ {:4} @ {:2f} ]"

class MarketController
{
    public:
    MarketController(std::shared_ptr<OrderBookService> orderBookService, std::shared_ptr<TradeTrackerService> tradeTrackerService,
                     std::shared_ptr<MarketServer> server);

    void GetMetrics(const std::string &userId);
    void PutOrder(const std::string &body);
    void match(int tick = 0);
    void Stop();

    private:
    std::shared_ptr<MarketServer>        server_;
    std::shared_ptr<OrderBookService>    orderBookService_;
    std::shared_ptr<TradeTrackerService> tradeTrackerService_;
};
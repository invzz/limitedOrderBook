#pragma once
#include <zmq.hpp>
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <shared_mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "common_topics.hh"

#include "orderBookService.hh"
#include "marketController.hh"
#include "marketMediator.hh"
#include "tradeTrackerService.hh"

#define PUB_ADDRESS    "tcp://*:5555"
#define PULL_ADDRESS   "tcp://*:5556"
#define ROUTER_ADDRESS "tcp://*:5557"

class MarketServer : public std::enable_shared_from_this<MarketServer>
{
    public:
    MarketServer();

    void initialize();
    void mainLoop();
    void start();
    void stop();
    void sendMessage(const std::string &userId, const std::string &content);

    std::shared_ptr<OrderBookService>    getOrderBookService() const { return orderBookService_; }
    std::shared_ptr<TradeTrackerService> getTradeTrackerService() const { return tradeTrackerService_; }

    private:
    void commandLoop();
    void publishOrderBook();
    void liquidatePositions();
    void generateReport();

    std::shared_ptr<OrderBookService>    orderBookService_;
    std::shared_ptr<MarketController>    controller_;
    std::shared_ptr<MarketMediator>      mediator_;
    std::shared_ptr<TradeTrackerService> tradeTrackerService_;

    zmq::context_t context_;
    zmq::socket_t  pubSocket_;
    zmq::socket_t  routerSocket_;

    std::atomic<bool> running_;
    std::thread       CommandListenerThread_;
    std::thread       MainLoopThread_;
    int               current_tick_;
    double            lastAvgPrice_;
};
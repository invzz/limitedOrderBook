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
#include "MultiOrderBookService.hh"
#include "MultiTradeTrackerService.hh"
#include "marketController.hh"
#include "marketServiceRouter.hh"

#include "common.hh"

#ifndef MAIN_LOOP_SLEEP_TIME
#define MAIN_LOOP_SLEEP_TIME 100
#endif

#ifndef ROUTER_SOCKET_TIMEOUT
#define ROUTER_SOCKET_TIMEOUT 100
#endif

namespace market
{

    class MultiMarketServer : public std::enable_shared_from_this<MultiMarketServer>
    {
        public:
        MultiMarketServer();

        void                                      initialize();
        void                                      mainLoop();
        void                                      start();
        void                                      stop();
        void                                      sendMessage(const std::string &userId, const std::string &content);
        std::shared_ptr<MultiOrderBookService>    getOrderBookService() const { return orderBookService_; }
        std::shared_ptr<MultiTradeTrackerService> getTradeTrackerService() const { return tradeTrackerService_; }

        private:
        void commandLoop();
        void publishOrderBook();
        void liquidatePositions();
        void generateReport();

        std::shared_ptr<MultiOrderBookService>                  orderBookService_;
        std::shared_ptr<MultiTradeTrackerService>               tradeTrackerService_;
        std::shared_ptr<MarketServiceRouter<MultiMarketServer>> router_;

        zmq::context_t context_;
        zmq::socket_t  pubSocket_;
        zmq::socket_t  routerSocket_;

        std::atomic<bool> running_;
        std::thread       CommandListenerThread_;
        std::thread       MainLoopThread_;
        int               current_tick_;
    };

} // namespace market
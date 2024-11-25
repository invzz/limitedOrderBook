#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <any>
#include <spdlog/spdlog.h>
#include "MultiOrderBookService.hh"
#include "MultiTradeTrackerService.hh"
#include "MarketMediator.hh"

class MultiMarketServer;

namespace market
{
    template <typename T> class MarketServiceRouter
    {
        public:
        MarketServiceRouter() : orderBookService_(std::make_shared<MultiOrderBookService>()), tradeTrackerService_(std::make_shared<MultiTradeTrackerService>()) { spdlog::info("[Router] Created"); }

        MarketServiceRouter(std::shared_ptr<MultiOrderBookService> orderBookService, std::shared_ptr<MultiTradeTrackerService> tradeTrackerService)
            : orderBookService_(std::move(orderBookService)), tradeTrackerService_(std::move(tradeTrackerService))
        {}

        void setServer(std::shared_ptr<T> server)
        {
            this->server_              = server;
            this->orderBookService_    = server->getOrderBookService();
            this->tradeTrackerService_ = server->getTradeTrackerService();
        }

        void processRequests(int currentTick)
        {
            for(const auto &[product, mediator] : mediators_)
                {
                    spdlog::debug("[ Router ] Processing requests for product: {}", market::yellow(product));
                    mediator->handleRequest("process_tick", currentTick);
                }
        }

        void handleCommand(const std::string &command, const std::string &body, const std::string &userId)
        {
            try
                {
                    auto jsonBody = nlohmann::json::parse(body);

                    if(!jsonBody.contains("product"))
                        {
                            spdlog::error("[ {} ] Command body does not contain 'product' key for command: {}", market::red(__func__), command);
                            return;
                        }

                    std::string product = jsonBody["product"];

                    std::shared_ptr<MarketMediator<T>> mediator = getOrCreate(product);

                    spdlog::debug("[ {} ] Routing command '{}' to mediator for product: {}", market::red(__func__), market::yellow(command), market::green(product));

                    mediator->handleRequest(command, jsonBody, userId);
                }
            catch(const nlohmann::json::exception &e)
                {
                    spdlog::error("[ {} ] Failed to parse command body as JSON: {}", market::red(__func__), e.what());
                }
        }

        std::shared_ptr<MarketMediator<T>> getOrCreate(const std::string &product)
        {
            if(mediators_.find(product) == mediators_.end())
                {
                    spdlog::info("[ {} ] Creating mediator for product: {}", market::red(__func__),  product);
                    auto s          = orderBookService_->getOrCreate(product);
                    auto t          = tradeTrackerService_->getOrCreate(product);
                    auto controller = std::make_shared<MarketController<T>>(s, t, server_);
                    auto mediator   = std::make_shared<MarketMediator<T>>();
                    mediator->setController(controller);
                    mediators_[product] = mediator;
                }
            return mediators_[product];
        }

        private:
        std::shared_ptr<MultiOrderBookService>                              orderBookService_;
        std::shared_ptr<MultiTradeTrackerService>                           tradeTrackerService_;
        std::unordered_map<std::string, std::shared_ptr<MarketMediator<T>>> mediators_;
        std::shared_ptr<T>                                                  server_;
    };

} // namespace market

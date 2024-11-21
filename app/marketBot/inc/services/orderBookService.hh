#pragma once
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "order.hh"
#include "priceLevelService.hh"
#include "trade.hh"
#include <vector>
#include <atomic>

class OrderBookService
{
    public:
    OrderBookService() : nextId_(0)
    {
        spdlog::info("[OrderBookService] Creating OrderBookService instance");
        bids_ = std::make_shared<PriceLevelService>();
        asks_ = std::make_shared<PriceLevelService>();
    }

    int generateId() { return nextId_++; }

    void addBid(std::shared_ptr<Order> order)
    {
        int id = generateId();
        order->setId(id);
        double price = order->getPrice();
        bids_->addOrder(price, std::move(order));
    }

    void addAsk(std::shared_ptr<Order> order)
    {
        int id = generateId();
        order->setId(id);
        double price = order->getPrice();
        asks_->addOrder(price, std::move(order));
    }

    void clear()
    {
        if(bids_ != nullptr && asks_ != nullptr)
            {
                bids_->clear();
                asks_->clear();
            }
    }

    nlohmann::json getAsJson() const
    {
        nlohmann::json json;
        json["bids"] = bids_->getAsJson();
        json["asks"] = asks_->getAsJson();
        return json;
    }

    std::vector<std::shared_ptr<Order>> getBestBid()
    {
        if(bids_ == nullptr) { return {}; }

        try
            {
                auto level = bids_->getHighestPriceLevel();
                if(level == nullptr) { return {}; }
                return level->getOrders();
            }
        catch(const std::runtime_error &e)
            {
                spdlog::warn("[OrderBookService] Failed to get best bid: {}", e.what());
                return {};
            }
    }

    std::vector<std::shared_ptr<Order>> getBestAsk()
    {
        if(asks_ == nullptr) { return {}; }

        try
            {
                auto level = asks_->getLowestPriceLevel();

                if(level == nullptr) { return {}; }

                return level->getOrders();
            }
        catch(const std::runtime_error &e)
            {
                spdlog::warn("[OrderBookService] Failed to get best ask: {}", e.what());
                return {};
            }
    }

    double getBestAskPrice()
    {
        if(asks_ == nullptr) { return 0; }

        try
            {
                return asks_->getLowestPrice();
            }
        catch(const std::runtime_error &e)
            {
                spdlog::warn("[OrderBookService] Failed to get best ask price: {}", e.what());
                return 0;
            }
    }

    double getBestBidPrice()
    {
        if(bids_ == nullptr) { return 0; }

        try
            {
                return bids_->getHighestPrice();
            }
        catch(const std::runtime_error &e)
            {
                spdlog::warn("[OrderBookService] Failed to get best bid price: {}", e.what());
                return 0;
            }
    }

    double getAvgPrice()
    {
        double highestBid = getBestBidPrice();
        double lowestAsk  = getBestAskPrice();
        if(highestBid == 0 || lowestAsk == 0) { return 0; }
        return (highestBid + lowestAsk) / 2;
    }

    size_t getTotalOrdersCount() { return bids_->getOrdersCount() + asks_->getOrdersCount(); }

    bool canMatch(double buy_price, double sell_price) { return buy_price >= sell_price; }

    void updateOrderBook(const nlohmann::json &orderBookData, bool doClean = false)
    {
        if(doClean) clear();

        updateOrders(orderBookData["bids"], bids_);
        updateOrders(orderBookData["asks"], asks_);
    }

    private:
    void updateOrders(const nlohmann::json &levelsData, std::shared_ptr<PriceLevelService> &priceLevelService)
    {
        for(const auto &levelData : levelsData)
            {
                auto level = PriceLevel::fromJson(levelData);
                for(const auto &order : level->getOrders())
                    {
                        if(order == nullptr || order->getQuantity() == 0) { continue; }
                        auto price = level->getPrice();
                        priceLevelService->addOrder(price, order);
                    }
            }
    }

    std::vector<std::shared_ptr<Trade>> match(int tick)
    {
        std::vector<std::shared_ptr<Trade>> trades;
        auto                                bids = bids_->getAsMap();
        auto                                asks = asks_->getAsMap();

        std::unique_lock<std::shared_mutex> lock(mtx_);

        for(auto it_buy = bids.begin(); it_buy != bids.end();)
            {
                double bid_price = it_buy->first;
                auto  &buyOrders = it_buy->second->getOrders();

                for(auto it_sell = asks.begin(); it_sell != asks.end();)
                    {
                        double ask_price = it_sell->first;

                        if(!canMatch(bid_price, ask_price))
                            {
                                ++it_sell;
                                continue;
                            }

                        auto &sellOrders = it_sell->second->getOrders();

                        matchOrders(tick, buyOrders, sellOrders, trades);

                        if(sellOrders.empty()) { it_sell = asks.erase(it_sell); }
                        else { ++it_sell; }

                        if(buyOrders.empty())
                            {
                                it_buy = bids.erase(it_buy);
                                break;
                            }
                    }

                if(it_buy != bids.end()) { ++it_buy; }
            }

        return trades;
    }

    void matchOrders(int tick, std::vector<std::shared_ptr<Order>> &buyOrders, std::vector<std::shared_ptr<Order>> &sellOrders,
                     std::vector<std::shared_ptr<Trade>> &trades)
    {
        for(auto buyOrderIt = buyOrders.begin(); buyOrderIt != buyOrders.end();)
            {
                auto &buyOrder = *buyOrderIt;

                for(auto sellOrderIt = sellOrders.begin(); sellOrderIt != sellOrders.end();)
                    {
                        auto &sellOrder = *sellOrderIt;

                        if(!sellOrder || !buyOrder || buyOrder->getUserId() == sellOrder->getUserId())
                            {
                                ++sellOrderIt;
                                continue;
                            }

                        int quantityTraded = std::min(buyOrder->getQuantity(), sellOrder->getQuantity());

                        buyOrder->updateQuantity(-quantityTraded);
                        sellOrder->updateQuantity(-quantityTraded);

                        auto trade = std::make_shared<Trade>(tick, buyOrder->getUserId(), sellOrder->getUserId(), sellOrder->getPrice(), quantityTraded);
                        trades.push_back(trade);

                        if(sellOrder->getQuantity() == 0) { sellOrderIt = sellOrders.erase(sellOrderIt); }
                        else { ++sellOrderIt; }

                        if(buyOrder->getQuantity() == 0)
                            {
                                buyOrderIt = buyOrders.erase(buyOrderIt);
                                break;
                            }
                    }

                if(buyOrderIt != buyOrders.end()) { ++buyOrderIt; }
            }
    }
    std::shared_mutex                  mtx_;
    std::shared_ptr<PriceLevelService> bids_;
    std::shared_ptr<PriceLevelService> asks_;
    std::atomic<int>                   nextId_;
};
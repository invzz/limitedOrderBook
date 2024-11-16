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

    nlohmann::json getAsJson() const
    {
        nlohmann::json json;
        json["bids"] = bids_->getAsJson();
        json["asks"] = asks_->getAsJson();
        return json;
    }

    std::vector<std::shared_ptr<Order>> getBestBid() { return bids_->getLowestPriceLevel()->getOrders(); }

    std::vector<std::shared_ptr<Order>> getBestAsk() { return asks_->getLowestPriceLevel()->getOrders(); }

    double getAvgPrice() { return (bids_->getLowestPrice() + asks_->getLowestPrice()) / 2; }

    size_t getTotalOrdersCount() { return bids_->getOrdersCount() + asks_->getOrdersCount(); }

    bool canMatch(double buy_price, double sell_price) { return buy_price >= sell_price; }

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

                        for(auto buyOrderIt = buyOrders.begin(); buyOrderIt != buyOrders.end();)
                            {
                                auto &buyOrder = *buyOrderIt;

                                for(auto sellOrderIt = sellOrders.begin(); sellOrderIt != sellOrders.end();)
                                    {
                                        auto &sellOrder = *sellOrderIt;

                                        if(!sellOrder || !buyOrder)
                                            {
                                                ++sellOrderIt;
                                                continue;
                                            }

                                        if(buyOrder->getUserId() == sellOrder->getUserId())
                                            {
                                                ++sellOrderIt;
                                                continue;
                                            }
                                        auto buyer          = buyOrder->getUserId();
                                        auto seller         = sellOrder->getUserId();
                                        int  quantityTraded = std::min(buyOrder->getQuantity(), sellOrder->getQuantity());

                                        buyOrder->updateQuantity(-quantityTraded);
                                        sellOrder->updateQuantity(-quantityTraded);

                                        auto trade = std::make_shared<Trade>(tick, buyer, seller, sellOrder->getPrice(), quantityTraded);
                                        trades.push_back(trade);

                                        if(sellOrder->getQuantity() == 0) { sellOrderIt = sellOrders.erase(sellOrderIt); }
                                        else { ++sellOrderIt; }

                                        if(buyOrder->getQuantity() == 0)
                                            {
                                                buyOrderIt = buyOrders.erase(buyOrderIt);
                                                break;
                                            }
                                    }
                                if(buyOrderIt == buyOrders.end()) { break; }
                                else { ++buyOrderIt; }
                            }
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

    private:
    std::shared_mutex                  mtx_;
    std::shared_ptr<PriceLevelService> bids_;
    std::shared_ptr<PriceLevelService> asks_;
    std::atomic<int>                   nextId_;
};
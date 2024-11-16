#pragma once
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "priceLevel.hh"
#include "priceLevelRepository.hh"
#include <spdlog/spdlog.h>
#include <vector>

class PriceLevelService
{
    public:
    PriceLevelService()
    {
        spdlog::info("[ {} ] Created", __func__);
        repository_ = std::make_shared<PriceLevelRepository>();
    }

    void addOrder(double price, std::shared_ptr<Order> order)
    {
        auto level = repository_->get(price);
        if(!level)
            {
                level = std::make_shared<PriceLevel>(price);
                repository_->add(price, level);
            }
        level->addOrder(std::move(order));
    }

    std::shared_ptr<PriceLevel> getLevel(double price) const { return repository_->get(price); }

    std::unordered_map<double, std::shared_ptr<PriceLevel>> getAsMap() const { return repository_->getAll(); }

    double getLowestPrice() const { return repository_->getLowestPrice(); }

    std::shared_ptr<PriceLevel> getLowestPriceLevel() const { return repository_->getLowestLevel(); }

    double getHighestPrice() const { return repository_->getHighestPrice(); }

    std::shared_ptr<PriceLevel> getHighestPriceLevel() const { return repository_->getHighestLevel(); }

    std::vector<std::shared_ptr<Order>> getOrdersAtPrice(double price) const
    {
        auto level = repository_->get(price);
        if(level) { return level->getOrders(); }
        return {};
    }

    size_t getOrdersCount() const { return repository_->getOrdersCount(); }

    nlohmann::json getAsJson() const { return repository_->getAsJson(); }

    private:
    std::shared_ptr<PriceLevelRepository> repository_;
};
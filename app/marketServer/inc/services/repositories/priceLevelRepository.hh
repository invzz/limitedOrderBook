#pragma once
#include "genericRepository.hh"
#include "priceLevel.hh"
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include <unordered_map>

class PriceLevelRepository : public GenericOrderedRepository<double, PriceLevel>
{
    public:
    void clear() override
    {
        std::unique_lock lock(mutex_);
        priceLevels_ = {};
    }

    void add(const double &price, std::shared_ptr<PriceLevel> level) override
    {
        std::unique_lock lock(mutex_);
        priceLevels_[price] = std::move(level);
    }

    void remove(const double &price) override
    {
        std::unique_lock lock(mutex_);
        priceLevels_.erase(price);
    }

    std::shared_ptr<PriceLevel> get(const double &price) const override
    {
        std::shared_lock lock(mutex_);
        auto             it = priceLevels_.find(price);
        if(it != priceLevels_.end()) { return it->second; }
        return nullptr;
    }

    std::map<double, std::shared_ptr<PriceLevel>> getAll() const override { return priceLevels_; }

    std::shared_ptr<PriceLevel> getLowestLevel() const { return get(getLowestPrice()); }
    std::shared_ptr<PriceLevel> getHighestLevel() const { return get(getHighestPrice()); }

    double getLowestPrice() const { return findLowestPriceLevel(); }

    double getHighestPrice() const { return findHighestPriceLevel(); }

    nlohmann::json getAsJson() const override
    {
        std::shared_lock lock(mutex_);
        nlohmann::json   json;
        for(const auto &[price, level] : priceLevels_) { json[std::to_string(price)] = level->getAsJson(); }
        return json;
    }

    size_t size() const
    {
        std::shared_lock lock(mutex_);
        return priceLevels_.size();
    }

    size_t getOrdersCount() const
    {
        std::shared_lock lock(mutex_);
        size_t           count = 0;
        for(const auto &[price, level] : priceLevels_) { count += level->size(); }
        return count;
    }

    private:
    double findLowestPriceLevel() const
    {
        std::shared_lock lock(mutex_);
        if(priceLevels_.empty()) { return 0; }
        return priceLevels_.begin()->first;
    }

    double findHighestPriceLevel() const
    {
        std::shared_lock lock(mutex_);
        if(priceLevels_.empty()) { return 0; }
        return priceLevels_.rbegin()->first;
    }

    mutable std::shared_mutex                     mutex_;
    std::map<double, std::shared_ptr<PriceLevel>> priceLevels_;
};

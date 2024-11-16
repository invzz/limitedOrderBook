#pragma once
#include "genericRepository.hh"
#include "priceLevel.hh"
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include <unordered_map>

class PriceLevelRepository : public GenericRepository<double, PriceLevel>
{
    public:
    void add(const double &price, std::shared_ptr<PriceLevel> level) override
    {
        std::unique_lock lock(mutex_);
        priceLevels_[price] = std::move(level);
        lowestPriceLevel_   = std::min(lowestPriceLevel_, price);
        highestPriceLevel_  = std::max(highestPriceLevel_, price);
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

    std::unordered_map<double, std::shared_ptr<PriceLevel>> getAll() const override { return priceLevels_; }

    std::shared_ptr<PriceLevel> getLowestLevel() const { return get(lowestPriceLevel_); }
    std::shared_ptr<PriceLevel> getHighestLevel() const { return get(highestPriceLevel_); }

    double getLowestPrice() const
    {
        std::shared_lock lock(mutex_);
        return lowestPriceLevel_;
    }

    double getHighestPrice() const
    {
        std::shared_lock lock(mutex_);
        return highestPriceLevel_;
    }

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
        auto             result = 0.0;
        for(const auto &[price, level] : priceLevels_)
            {
                if(price < lowestPriceLevel_) { result = price; }
            }
        return result;
    }

    double findHighestPriceLevel() const
    {
        std::shared_lock lock(mutex_);
        auto             result = 0.0;
        for(const auto &[price, level] : priceLevels_)
            {
                if(price < lowestPriceLevel_) { result = price; }
            }
        return result;
    }

    mutable std::shared_mutex                               mutex_;
    std::unordered_map<double, std::shared_ptr<PriceLevel>> priceLevels_;
    double                                                  lowestPriceLevel_  = 0;
    double                                                  highestPriceLevel_ = 0;
};
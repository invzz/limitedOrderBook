#pragma once
#include "genericRepository.hh"
#include "trade.hh"
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include <unordered_map>
#include <spdlog/spdlog.h>

class TradeRepository
{
    public:
    TradeRepository() { spdlog::info("[TradeRepository] :: Creating TradeRepository instance"); }

    void add(const int &id, std::shared_ptr<Trade> entity)
    {
        std::unique_lock lock(mutex_);
        entities_.emplace_back(std::move(entity));
    }

    std::vector<std::shared_ptr<Trade>> getAll() const
    {
        std::shared_lock lock(mutex_);
        return entities_;
    }

    nlohmann::json getAsJson() const
    {
        std::shared_lock lock(mutex_);
        nlohmann::json   j = nlohmann::json::array();
        for(const auto &entity : entities_) { j.push_back(entity->getAsJson()); }
        return j;
    }

    private:
    mutable std::shared_mutex           mutex_;
    std::vector<std::shared_ptr<Trade>> entities_;
};
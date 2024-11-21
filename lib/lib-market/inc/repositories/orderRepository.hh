#pragma once
#include "genericRepository.hh"
#include "order.hh"
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include <unordered_map>
namespace market
{
    class OrderRepository : public GenericUnorderedRepository<int, Order>
    {
        public:
        void add(const int &id, std::shared_ptr<Order> entity) override
        {
            std::unique_lock lock(mutex_);
            entities_[id] = std::move(entity);
        }

        void remove(const int &id) override
        {
            std::unique_lock lock(mutex_);
            entities_.erase(id);
        }

        std::shared_ptr<Order> get(const int &id) const override
        {
            std::shared_lock lock(mutex_);
            auto             it = entities_.find(id);
            if(it != entities_.end()) { return it->second; }
            return nullptr;
        }

        std::unordered_map<int, std::shared_ptr<Order>> getAll() const override
        {
            std::shared_lock lock(mutex_);
            return entities_;
        }

        nlohmann::json getAsJson() const override
        {
            std::shared_lock lock(mutex_);
            nlohmann::json   json;
            for(const auto &[id, entity] : entities_) { json[std::to_string(id)] = entity->getAsJson(); }
            return json;
        }

        private:
        mutable std::shared_mutex                       mutex_;
        std::unordered_map<int, std::shared_ptr<Order>> entities_;
    };
} // namespace market
#pragma once
#include <memory>
#include <unordered_map>
#include <shared_mutex>
#include <string>
#include <nlohmann/json.hpp>

template <typename Key, typename Value> class GenericRepository
{
    public:
    virtual ~GenericRepository() = default;

    virtual void                                            add(const Key &key, std::shared_ptr<Value> entity) = 0;
    virtual void                                            remove(const Key &key)                             = 0;
    virtual std::shared_ptr<Value>                          get(const Key &key) const                          = 0;
    virtual std::unordered_map<Key, std::shared_ptr<Value>> getAll() const                                     = 0;
    virtual nlohmann::json                                  getAsJson() const                                  = 0;
};

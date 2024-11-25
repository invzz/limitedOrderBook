// commandFactory.hh
#pragma once
#include "command.hh"
#include "getMetricsCommand.hh"
#include "marketController.hh"
#include "matchOrdersCommand.hh"
#include "putOrderCommand.hh"
#include "spdlog/spdlog.h"
#include "stopCommand.hh"
#include <functional>
#include <memory>
#include <unordered_map>

namespace market
{
    struct CommandParams
    {
        std::string command;
        std::string userId;
    };

    template <typename T> class CommandFactory
    {
        public:
        static std::unique_ptr<BaseCommand> createCommand(const CommandParams &params, std::shared_ptr<MarketController<T>> controller);

        private:
        static const std::unordered_map<std::string, std::function<std::unique_ptr<BaseCommand>(std::shared_ptr<MarketController<T>>, const std::string &)>> commandMap;
    };

    template <typename T>
    const std::unordered_map<std::string, std::function<std::unique_ptr<BaseCommand>(std::shared_ptr<MarketController<T>>, const std::string &)>> CommandFactory<T>::commandMap = {
      {"GET_METRICS",  [](std::shared_ptr<MarketController<T>> controller, const std::string &userId) { return std::make_unique<GetMetricsCommand<T>>(controller, userId); }},
      {"PUT_ORDER",    [](std::shared_ptr<MarketController<T>> controller, const std::string &) { return std::make_unique<PutOrderCommand<T>>(controller); }                },
      {"STOP",         [](std::shared_ptr<MarketController<T>> controller, const std::string &) { return std::make_unique<StopCommand<T>>(controller); }                    },
      {"MATCH_ORDERS", [](std::shared_ptr<MarketController<T>> controller, const std::string &) { return std::make_unique<MatchOrdersCommand<T>>(controller); }             },
    };

    template <typename T> std::unique_ptr<BaseCommand> CommandFactory<T>::createCommand(const CommandParams &params, std::shared_ptr<MarketController<T>> controller)
    {
        auto it = commandMap.find(params.command);
        if(it != commandMap.end()) { return it->second(controller, params.userId); }
        else { return nullptr; }
    }
} // namespace market

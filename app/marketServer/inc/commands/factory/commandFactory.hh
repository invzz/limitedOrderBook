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

class CommandFactory
{
    public:
    using CommandCreator = std::function<std::unique_ptr<BaseCommand>(std::shared_ptr<MarketController>, const std::string &)>;
    using CommandMap     = std::unordered_map<std::string, CommandCreator>;

    static std::unique_ptr<BaseCommand> createCommand(const std::string &command, const std::string &userId, std::shared_ptr<MarketController> controller);

    private:
    static const CommandMap commandMap;
};


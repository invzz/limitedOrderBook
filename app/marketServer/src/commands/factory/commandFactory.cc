#include <commandFactory.hh>

const CommandFactory::CommandMap CommandFactory::commandMap = {
  {"GET_METRICS",
   [](std::shared_ptr<MarketController> controller,                 const std::string &userId) { return std::make_unique<GetMetricsCommand>(controller, userId); }},
  {"PUT_ORDER",    [](std::shared_ptr<MarketController> controller, const std::string &) { return std::make_unique<PutOrderCommand>(controller); }                },
  {"STOP",         [](std::shared_ptr<MarketController> controller, const std::string &) { return std::make_unique<StopCommand>(controller); }                    },
  {"MATCH_ORDERS", [](std::shared_ptr<MarketController> controller, const std::string &) { return std::make_unique<MatchOrdersCommand>(controller); }             }
};

std::unique_ptr<BaseCommand> CommandFactory::createCommand(const std::string &command, const std::string &userId, std::shared_ptr<MarketController> controller)
{
    auto it = commandMap.find(command);
    if(it != commandMap.end()) { return it->second(controller, userId); }
    else { return nullptr; }
}
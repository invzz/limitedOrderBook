#pragma once
#include "command.hh"
#include <memory>
#include "marketController.hh"
#include "spdlog/spdlog.h"

class CommandFactory
{
  public:
  static std::unique_ptr<Command> createCommand(const std::string &command, const std::string &userId, MarketServer *server)
  {
    if(command == GET_METRICS)
      {
        spdlog::debug("[Server] Creating GetMetricsCommand");
        return std::make_unique<GetMetricsCommand>(server, userId);
      }

    else if(command == "PUT_ORDER")
      {
        spdlog::debug("[Server] Creating PutOrderCommand");
        return std::make_unique<PutOrderCommand>(server);
      }

    else if(command == "STOP")
      {
        spdlog::debug("[Server] Creating StopCommand");
        return std::make_unique<StopCommand>(server);
      }

    else
      {
        spdlog::warn("[Server] Unknown command: {}", command);
        return nullptr;
      }
  }
};
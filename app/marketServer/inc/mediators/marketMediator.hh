#pragma once
#include "command.hh"
#include "commandFactory.hh"
#include "marketController.hh"
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <any>

class MarketMediator
{
    public:
    MarketMediator() { spdlog::info("[ {} ] Created", __func__); }

    void handleRequest(const std::string &commandType, const std::any &body, const std::string &userId = "")
    {
        CommandParams params{commandType, userId};
        auto          command = CommandFactory::createCommand(params, controller_);
        if(command) { command->execute(body); }
        else { spdlog::error("Unknown command type: {}", commandType); }
    }

    void setController(std::shared_ptr<MarketController> controller) { controller_ = controller; }

    private:
    std::shared_ptr<MarketController> controller_;
};
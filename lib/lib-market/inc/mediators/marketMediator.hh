#pragma once
#include "command.hh"
#include "commandFactory.hh"
#include "marketController.hh"
#include "custom_formatter.hh"
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <any>
namespace market
{
    template <typename T> class MarketMediator
    {
        public:
        MarketMediator() { spdlog::info("[ {} ] Created", market::red(__func__)); }

        void handleRequest(const std::string &commandType, const std::any &body, const std::string &userId = "")
        {
            spdlog::debug("[ {} ] Handling command: {} from user: {}", market::red(__func__), market::yellow(commandType), userId);
            CommandParams params{commandType, userId};
            auto          command = CommandFactory<T>::createCommand(params, controller_);
            if(command) { command->execute(body); }
            else { spdlog::error("Unknown command type: {}", commandType); }
        }
        template <typename T> void setController(std::shared_ptr<MarketController<T>> controller) { controller_ = controller; }

        private:
        std::shared_ptr<MarketController<T>> controller_;
    };
} // namespace market
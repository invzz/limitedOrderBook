
#pragma once
#include <string>
#include <memory>
#include <any>
#include "command.hh"
#include "marketController.hh"
namespace market
{
    /**
     * @class PutOrderCommand
     * @brief A command to put an order in the market.
     *
     * This class inherits from the Command class with a std::string type parameter.
     * It is used to execute the action of putting an order in the market through the MarketController.
     *
     * @tparam std::string The type of the command parameter.
     */
    class PutOrderCommand : public Command<std::string>
    {
        public:
        PutOrderCommand(std::shared_ptr<MarketController> controller) : controller_(controller) {}

        void execute(const std::string &body) override { controller_->PutOrder(body); }

        private:
        std::shared_ptr<MarketController> controller_;
    };
} // namespace market
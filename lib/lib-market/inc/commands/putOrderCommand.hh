
#pragma once
#include <string>
#include <memory>
#include <any>
#include "command.hh"
#include "marketController.hh"
#include "nlohmann/json.hpp"
namespace market
{
    /**
     * @class PutOrderCommand
     * @brief A command to put an order in the market.
     *
     * This class inherits from the Command class with a std::string type parameter.
     * It is used to execute the action of putting an order in the market through the MarketController<T>.
     *
     * @tparam std::string The type of the command parameter.
     */
    template <typename T> class PutOrderCommand : public Command<nlohmann::json, T>
    {
        public:
        PutOrderCommand(std::shared_ptr<MarketController<T>> controller) : controller_(controller) {}

        void execute(const nlohmann::json &body) override { controller_->PutOrder(body); }

        private:
        std::shared_ptr<MarketController<T>> controller_;
    };
} // namespace market

#pragma once
#include <string>
#include <memory>
#include <any>
#include "command.hh"
#include "marketController.hh"
namespace market
{
    /**
     * @class MatchOrdersCommand
     * @brief A command to match orders in the market.
     *
     * This class inherits from the Command class with an integer template parameter.
     * It is used to execute the matching of orders in the market by invoking the
     * match function on the MarketController instance.
     *
     * @tparam int The type of the parameter for the Command base class.
     */

    class MatchOrdersCommand : public Command<int>
    {
        public:
        MatchOrdersCommand(std::shared_ptr<MarketController> controller) : controller_(controller) {}

        void execute(const int &tick) override
        {
            if(tick >= 0) { controller_->match(tick); }
            else
                {
                    // Handle invalid tick_ value, e.g., log an error or throw an exception
                    throw std::invalid_argument("Invalid tick value");
                }
        }

        private:
        std::shared_ptr<MarketController> controller_;
    };
} // namespace market
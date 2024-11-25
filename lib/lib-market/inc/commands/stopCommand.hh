#pragma once
#include <string>
#include <memory>
#include <any>
#include "command.hh"
#include "marketController.hh"
namespace market
{
    /**
     * @class StopCommand
     * @brief A command to stop the market controller.
     *
     * This class inherits from the Command class with a std::string template parameter.
     * It is used to execute a stop command on the MarketController<T>.
     *
     * @tparam std::string The type of the command body.
     */
    template <typename T> class StopCommand : public Command<std::string, T>
    {
        public:
        StopCommand(std::shared_ptr<MarketController<T>> controller) : controller_(controller) {}

        void execute(const std::string &body) override { controller_->Stop(); }

        private:
        std::shared_ptr<MarketController<T>> controller_;
    };
} // namespace market
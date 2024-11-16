#pragma once
#include <string>
#include <memory>
#include <any>
#include "command.hh"
#include "marketController.hh"

/**
 * @class StopCommand
 * @brief A command to stop the market controller.
 *
 * This class inherits from the Command class with a std::string template parameter.
 * It is used to execute a stop command on the MarketController.
 *
 * @tparam std::string The type of the command body.
 */
class StopCommand : public Command<std::string>
{
    public:
    StopCommand(std::shared_ptr<MarketController> controller) : controller_(controller) {}

    void execute(const std::string &body) override { controller_->Stop(); }

    private:
    std::shared_ptr<MarketController> controller_;
};
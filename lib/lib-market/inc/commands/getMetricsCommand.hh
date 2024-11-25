
#pragma once
#include <string>
#include <memory>
#include <any>
#include "command.hh"
#include "marketController.hh"
namespace market
{
    /**
     * @class GetMetricsCommand
     * @brief A command to retrieve metrics for a specific user from the MarketController.
     *
     * This class inherits from the Command class with a std::string template parameter.
     * It is used to execute a command that retrieves metrics for a given user.
     *
     * @param controller_ A pointer to the MarketController instance.
     * @param userId_ The ID of the user for whom the metrics are to be retrieved.
     */
    template <typename T> class GetMetricsCommand : public Command<std::string, T>
    {
        public:
        GetMetricsCommand(std::shared_ptr<MarketController<T>> controller, const std::string &userId) : controller_(controller), userId_(userId) {}

        void execute(const std::string &body) override { controller_->GetMetrics(userId_); }

        private:
        std::shared_ptr<MarketController<T>> controller_;
        std::string                          userId_;
    };
} // namespace market
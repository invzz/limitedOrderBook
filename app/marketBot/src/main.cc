#include "simpleBot.hh"
#include "smartBot.hh"
#include "abstractBot.hh"
#include "OrderType.hh"
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>

int main()
{
    try
        {
            // if compiled in debug mode, set the log level to debug
            spdlog::set_level(spdlog::level::info);
            const auto *serverAddress = "tcp://localhost";

            std::vector<std::unique_ptr<Bot>> bots;
            bots.push_back(std::make_unique<SimpleBot>(serverAddress, "buyer", OrderType::BUY));
            bots.push_back(std::make_unique<SimpleBot>(serverAddress, "seller", OrderType::SELL));
            bots.push_back(std::make_unique<SmartBot>(serverAddress, "smarter"));

            for(auto &bot : bots) { bot->start(); }

            while(true) {}

            return 0;
        }
    catch(const std::exception &e)
        {
            spdlog::error("Exception: {}", e.what());
            return 1;
        }
}

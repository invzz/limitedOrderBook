#include "simpleBot.hh"
#include "smartBot.hh"
#include "abstractBot.hh"
#include "orderType.hh"
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>
#include <thread>
#include <iostream>

int main()
{
    try
        {
            // if compiled in debug mode, set the log level to debug
            spdlog::set_level(spdlog::level::debug);
            const auto                       *serverAddress = "tcp://localhost";
            std::vector<std::unique_ptr<Bot>> bots;
            bots.push_back(std::make_unique<SimpleBot>(serverAddress, "buyer", OrderType::BUY));
            bots.push_back(std::make_unique<SimpleBot>(serverAddress, "seller", OrderType::SELL));
            bots.push_back(std::make_unique<SmartBot>(serverAddress, "smarter"));
            std::vector<std::thread> botThreads;
            // create a thread for each bot and start
            for(auto &bot : bots)
                {
                    std::thread botThread([&bot]() {
                        {
                            bot->start();
                        }
                    });
                    botThreads.push_back(std::move(botThread));
                }
            std::cin.get();
            // stop the bots
            for(auto &bot : bots) { bot->stop(); }
            for(auto &botThread : botThreads) { botThread.join(); }
            return 0;
        }
    catch(const std::exception &e)
        {
            spdlog::error("Exception: {}", e.what());
            return 1;
        }
}

#include <memory>
#include <vector>
#include <thread>
#include <iostream>

#include "multiSimple.hh"
#include "smartBot.hh"
#include "MultiMarketClient.hh"

#include "orderType.hh"
#include <spdlog/spdlog.h>

using namespace market;

int main()
{
    try
        {
            // if compiled in debug mode, set the log level to debug
            spdlog::set_level(spdlog::level::info);
            const auto                                     *SERVER_ADDRESS = "tcp://localhost:5555";
            std::vector<std::unique_ptr<MultiMarketClient>> bots;

            for(int i = 0; i < 50; i++)
                {
                    bots.push_back(std::make_unique<MultiSimpleBot>("buyer" + std::to_string(i), OrderType::BUY));
                    bots.push_back(std::make_unique<MultiSimpleBot>("seller" + std::to_string(i), OrderType::SELL));
                }

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

#include "marketServer.hh"
#include <iostream>
#include <thread>
#include <exception>
#include <chrono>
#include <spdlog/spdlog.h>
using namespace market;

int main()
{
    try
        {
            spdlog::set_level(spdlog::level::debug);

            spdlog::info("*** [ main ] MARKET SERVER ***");

            std::shared_ptr<MarketServer> server = std::make_shared<MarketServer>();

            server->initialize(); // Ensure the controller is initialized after the MarketServer is managed by a shared_ptr

            std::thread serverThread([&server]() { server->start(); });

            std::cin.get();

            server->stop();

            serverThread.join(); // Ensure the server thread is joined before exiting
        }
    catch(const std::exception &e)
        {
            spdlog::error("Exception caught in main: {}", e.what());
            return 1;
        }

    return 0;
}
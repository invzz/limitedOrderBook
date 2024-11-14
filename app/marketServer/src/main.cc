#include "marketServer.hh"
#include <spdlog/spdlog.h>
#include <cxxopts.hpp>

int main(int argc, char *argv[])
{
  spdlog::set_level(spdlog::level::debug);

  auto seconds = 500;

  // cxxopts::Options options("yams", "Yet Another Market Simulator");

  //  options.add_options()("s,integer", "number of seconds", cxxopts::value<int>());

  //  auto result = options.parse(argc, argv);

  // auto seconds = result["s"].as<int>();

  // Create the market server
  MarketServer server;

  // Start the server in a separate thread
  // std::thread serverThread([&server]() { server.start(); });

  server.start();

  while(true) {};

  // std::this_thread::sleep_for(std::chrono::seconds(seconds));

  // Stop the server
  // std::cout << "*** [ main ] Stopping server..." << std::endl;

  server.stop();

  // Join the server thread to make sure it shuts down properly
  //if(serverThread.joinable()) { serverThread.join(); }

  std::cout << "*** [ main ] Server stopped." << std::endl;

  return 0;
}
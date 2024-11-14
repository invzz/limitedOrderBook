#include "marketServer.hh"
#include <spdlog/spdlog.h>
#include <cxxopts.hpp>

int main(int argc, char *argv[])
{
  spdlog::set_level(spdlog::level::info);

  auto seconds = 5;

  // cxxopts::Options options("yams", "Yet Another Market Simulator");

  //  options.add_options()("s,integer", "number of seconds", cxxopts::value<int>());

  //  auto result = options.parse(argc, argv);

  // auto seconds = result["s"].as<int>();

  // Create the market server
  MarketServer server;

  // Start the server in a separate thread
  std::cout << "*** [ main ] Starting server..." << std::endl;
  std::thread serverThread([&server]() { server.start(); });

  std::this_thread::sleep_for(std::chrono::seconds(seconds));

  // Stop the server
  std::cout << "*** [ main ] Stopping server..." << std::endl;

  

  // Join the server thread to make sure it shuts down properly
  if(serverThread.joinable()) {
      server.stop();
      serverThread.join(); }

  std::cout << "*** [ main ] Server stopped." << std::endl;

  return 0;
}
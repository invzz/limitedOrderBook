#include "marketServer.hh"
#include <spdlog/spdlog.h>

int main()
{
  // Create the market server
  MarketServer server;

  // Start the server in a separate thread
  std::thread serverThread([&server]() { server.start(); });

  // Simulate running the server for a certain period, for example 10 seconds
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Stop the server
  std::cout << "*** [ main ] Stopping server..." << std::endl;
  server.stop();

  // Join the server thread to make sure it shuts down properly
  if(serverThread.joinable()) { serverThread.join(); }

  std::cout << "*** [ main ] Server stopped." << std::endl;

  return 0;
}
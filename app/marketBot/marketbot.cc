#include "simpleBot.hh"
#include <spdlog/spdlog.h>

int main()
{
  // if compiled in debug mode, set the log level to debug
  spdlog::set_level(spdlog::level::info);
  std::string serverAddress = "tcp://localhost"; // Replace with your server address
  SimpleBot   buyerBot(serverAddress, 1, OrderType::BUY);
  SimpleBot   sellerBot(serverAddress, 2, OrderType::SELL);
  buyerBot.start();
  sellerBot.start();
  while(true) {}
  buyerBot.stop();
  return 0;
}

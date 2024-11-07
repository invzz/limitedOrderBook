#include "simpleBot.hh"
#include "smartBot.hh"
#include <spdlog/spdlog.h>

int main()
{
  // if compiled in debug mode, set the log level to debug
  spdlog::set_level(spdlog::level::info);
  std::string serverAddress = "tcp://localhost"; // Replace with your server address
  SimpleBot   buyerBot(serverAddress, "buyer", -1, OrderType::BUY);
  SimpleBot   sellerBot(serverAddress, "seller", -2, OrderType::SELL);
  SimpleBot   sellerBot2(serverAddress, "seller", -3, OrderType::SELL);
  SmartBot    smartBot(serverAddress, "smarter", 4);
  buyerBot.start();
  sellerBot.start();
  //sellerBot2.start();
  smartBot.start();
  while(true) {}
  buyerBot.stop();
  return 0;
}

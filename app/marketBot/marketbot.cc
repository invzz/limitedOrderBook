#include "simpleBot.hh"
#include <spdlog/spdlog.h>

int main()
{
  spdlog::set_level(spdlog::level::debug);
  std::string serverAddress = "tcp://localhost"; // Replace with your server address
  SimpleBot   myBot(serverAddress, 1, OrderType::BUY);
  SimpleBot   myBot2(serverAddress, 2, OrderType::SELL);
  myBot.start();
  myBot2.start();
  while(true) {}
  myBot.stop();
  return 0;
}

// int main()
// {
//   std::string orderBookJson = R"(
//         {
//             "buy_orders": [
//                 {
//                     "price": 100.0,
//                     "orders": [
//                         {"id": 3, "type": "BUY", "price": 100.0, "quantity": 10, "userId": 1},
//                         {"id": 7, "type": "BUY", "price": 100.0, "quantity": 10, "userId": 1}
//                     ]
//                 }
//             ],
//             "sell_orders": [
//                 {
//                     "price": 100.0,
//                     "orders": [
//                         {"id": 1, "type": "SELL", "price": 100.0, "quantity": 10, "userId": 1}
//                     ]
//                 }
//             ]
//         }
//     )";

//   nlohmann::json parsedJson = nlohmann::json::parse(orderBookJson);
//   auto           orderBook  = OrderBook::createFromJson(parsedJson);

//   // Use orderBook as needed
//   return 0;
// }
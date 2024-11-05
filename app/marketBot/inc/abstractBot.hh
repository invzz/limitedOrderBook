#include <zmq.hpp>
#include <thread>
#include <iostream>
#include <nlohmann/json.hpp>
#include "order.hh" // Include your Order class
#include "orderBook.hh"
#include "spdlog/spdlog.h"

class Bot
{
  public:
  Bot(const std::string &serverAddress)
      : context(1), orderSocket(context, ZMQ_PUSH), subscriberSocket(context, ZMQ_SUB)
  {
    orderSocket.connect(serverAddress + ":5555");      // Connect to the order receiving socket
    subscriberSocket.connect(serverAddress + ":5556"); // Connect to the order book updates
    subscriberSocket.set(zmq::sockopt::subscribe, ""); // Subscribe to all messages
  }
  void start()
  {
    spdlog::info("[Bot] Starting Bot");
    botThread = std::thread(&Bot::listenForUpdates, this);
  }
  void stop()
  {
    if(botThread.joinable())
      {
        running = false;
        botThread.join();
      }
  };

  protected:
  virtual void run() = 0; // Pure virtual function to be implemented by derived classes
  void         sendOrder(const nlohmann::json &order)
  {
    zmq::message_t orderMessage(order.dump().size());
    memcpy(orderMessage.data(), order.dump().data(), order.dump().size());
    orderSocket.send(orderMessage, zmq::send_flags::none);
    spdlog::info("Sent order: {}", order.dump());
  }

  void processOrderBookUpdate(const std::string &update)
  {
    try
      {
        // Parse the update JSON and populate the local order book
        nlohmann::json parsedUpdate = nlohmann::json::parse(update);

        if(parsedUpdate.is_array() && parsedUpdate.empty()) return; // Skip empty updates

        // Clear the local order book before adding the new orders
        orderBook.clear(); // Assuming you have a clear method to reset it

        // Now add the new orders to the local order book based on the update
        for(const auto &order : parsedUpdate["orders"]) // Adjust based on your JSON structure
          {
            OrderType type     = order["type"] == "BUY" ? OrderType::BUY : OrderType::SELL;
            double    price    = order["price"];
            int       quantity = order["quantity"];
            int       userId   = order["userId"];
            auto      newOrder = std::make_unique<Order>(type, price, quantity, userId);
            orderBook.addOrder(std::move(newOrder));
          }

        spdlog::info("Orderbook :: {}", update);
      }
    catch(const nlohmann::json::exception &e)
      {
        spdlog::error("Failed to process order book update: {} - {}", e.what(), update);
      }
  }

  void listenForUpdates()
  {
    while(running)
      {
        zmq::message_t update;
        if(subscriberSocket.recv(update, zmq::recv_flags::none))
          {
            std::string orderBookUpdate(static_cast<char *>(update.data()), update.size());
            processOrderBookUpdate(orderBookUpdate);
            run(); // Run the bot logic after processing the order book update
          }
      }
  }

  private:
  zmq::context_t context;
  zmq::socket_t  orderSocket; // For sending orders to the server
  zmq::socket_t  subscriberSocket;

  std::atomic<bool> running{true}; // Control running state
  std::thread       botThread;     // Thread for the bot's execution

  OrderBook orderBook; // For receiving order book updates
};
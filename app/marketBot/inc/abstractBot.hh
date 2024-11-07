#pragma once
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
  Bot(const std::string &serverAddress, std::string name, int userId)
      : context(1), orderSocket(context, ZMQ_PUSH), subscriberSocket(context, ZMQ_SUB), name(name), userId(userId)
  {
    orderSocket.connect(serverAddress + ":5555");      // Connect to the order receiving socket
    subscriberSocket.connect(serverAddress + ":5556"); // Connect to the order book updates
    subscriberSocket.set(zmq::sockopt::subscribe, ""); // Subscribe to all messages
  }
  void start()
  {
    spdlog::info("Starting Bot {}", name);
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

  int         getUserId() const { return userId; }
  std::string getName() const { return name; }

  protected:
  std::unique_ptr<OrderBook> orderBook;

  virtual void run() = 0; // Pure virtual function to be implemented by derived classes
  void         sendOrder(const nlohmann::json &order)
  {
    zmq::message_t orderMessage(order.dump().size());
    memcpy(orderMessage.data(), order.dump().data(), order.dump().size());
    orderSocket.send(orderMessage, zmq::send_flags::none);
    spdlog::info("[ {} ] Sent order:\n{}", name, order.dump(4));
  }

  void processOrderBookUpdate(const std::string &update)
  {
    try
      {
        nlohmann::json orderBookUpdate = nlohmann::json::parse(update);

        // check if the order book is empty
        if(orderBookUpdate.empty())
          {
            spdlog::warn("[ {} ] Received empty order book update", name);
            return;
          }

        orderBook = OrderBook::fromJson(orderBookUpdate);

        spdlog::debug("[ {} ] Received order book update: {} ", name, orderBook->totalOrders());
      }
    catch(const nlohmann::json::exception &e)
      {
        spdlog::error("[ {} ] Failed to process order book update: {} - {}", name, e.what(), update);
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
  zmq::context_t    context;
  zmq::socket_t     orderSocket; // For sending orders to the server
  zmq::socket_t     subscriberSocket;
  std::string       name;
  int               userId;
  std::atomic<bool> running{true}; // Control running state
  std::thread       botThread;     // Thread for the bot's execution
};
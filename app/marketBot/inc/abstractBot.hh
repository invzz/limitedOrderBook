#pragma once
#include <zmq.hpp>
#include <thread>
#include <iostream>
#include <nlohmann/json.hpp>
#include <mutex>
#include "order.hh"
#include "orderBook.hh"
#include "spdlog/spdlog.h"
#include "common_topics.hh"

#define PULL_ADDRESS   "tcp://localhost:5555"
#define ROUTER_ADDRESS "tcp://localhost:5557"

class Bot
{
  public:
  Bot(const std::string &serverAddress, std::string userId) : context(1), subSocket(context, ZMQ_SUB), dealerSocket(context, ZMQ_DEALER), userId(userId)
  {
    subSocket.connect(PULL_ADDRESS);
    subSocket.set(zmq::sockopt::subscribe, BOOK_TOPIC);
    dealerSocket.connect(ROUTER_ADDRESS);
    dealerSocket.set(zmq::sockopt::routing_id, userId);
  }

  virtual ~Bot() { stop(); }

  void start()
  {
    spdlog::info("Starting Bot {}", userId);
    running = true;
    // subThread    = std::thread(&Bot::listenForOrderBookSub, this);
    dealerThread = std::thread(&Bot::listenForMetrics, this);
  }

  void stop()
  {
    if(subThread.joinable())
      {
        running = false;
        subThread.join();
      }
    if(dealerThread.joinable())
      {
        running = false;
        dealerThread.join();
      }
  }

  virtual void run() = 0;

  void doRun() { run(); }

  std::string getUserId() const { return userId; }

  protected:
  std::shared_ptr<OrderBook> orderBook;

  void putOrder(const nlohmann::json &order)
  {
    std::string    message = "PUT_ORDER " + userId + " " + order.dump();
    zmq::message_t request(message.c_str(), message.size());
    dealerSocket.send(request, zmq::send_flags::none);
    spdlog::info("[ {} ] Sent order:\n{}", userId, order.dump(4));
    getMetrics();
  }

  void getMetrics()
  {
    std::string    message = "GET_METRICS " + userId;
    zmq::message_t request(message.c_str(), message.size());
    dealerSocket.send(request, zmq::send_flags::none);
    spdlog::info("[ {} ] Requested metrics", userId);
  }

  void updateOrderBook(const std::string &update)
  {
    try
      {
        nlohmann::json orderBookUpdate = nlohmann::json::parse(update);

        if(orderBookUpdate.is_array() && orderBookUpdate.empty())
          {
            spdlog::info("[ {} ] Received empty order book update", userId);
            orderBook = std::make_unique<OrderBook>();
          }
        else
          {
            spdlog::info("[ {} ] Received order book update! ", userId);
            orderBook = OrderBook::fromJson(orderBookUpdate);
          }
      }
    catch(const nlohmann::json::exception &e)
      {
        spdlog::error("[ {} ] Failed to process order book update: {} - {}", userId, e.what(), update);
      }
  }

  void updateMetrics(const std::string &update)
  {
    try
      {
        nlohmann::json metricsUpdate = nlohmann::json::parse(update);
        spdlog::info("[ {} ] Received metrics update!", userId);
      }
    catch(const nlohmann::json::exception &e)
      {
        spdlog::error("[ {} ] Failed to process metrics update: {} - {}", userId, e.what(), update);
      }
  }

  private:
  std::atomic<bool> running{true};
  zmq::context_t    context;
  zmq::socket_t     subSocket;
  zmq::socket_t     dealerSocket;
  std::thread       subThread;
  std::thread       dealerThread;
  std::string       userId;

  void listenForMetrics()
  {
    while(running)
      {
        zmq::message_t message;
        {
          getMetrics();
          if(dealerSocket.recv(message, zmq::recv_flags::dontwait) && dealerSocket.recv(message, zmq::recv_flags::none))
            {
              std::string receivedMessage(static_cast<char *>(message.data()), message.size());
              spdlog::debug("[ {} ] Received metrics: {}", userId, receivedMessage);
              updateMetrics(receivedMessage);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
  }

  void listenForOrderBookSub()
  {
    while(running)
      {
        zmq::message_t topic;
        zmq::message_t message;
        {
          if(subSocket.recv(topic, zmq::recv_flags::dontwait) && subSocket.recv(message, zmq::recv_flags::none))
            {
              std::string receivedMessage(static_cast<char *>(message.data()), message.size());
              spdlog::debug("[ {} ] Received message: {}", userId, receivedMessage);
              updateOrderBook(receivedMessage);
              doRun();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
  }
};
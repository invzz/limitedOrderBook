#include <zmq.hpp>
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <shared_mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "orderBook.hh"
#include "order.hh"
#include "metrics.hh"

class MarketServer
{
  public:
  MarketServer()
      : context(1), pullSocket(context, ZMQ_PULL), pubSocket(context, ZMQ_PUB), running(false)
  {
    pullSocket.bind("tcp://*:5555"); // For receiving orders
    pubSocket.bind("tcp://*:5556");  // For publishing order book updates
  }

  ~MarketServer() { stop(); }

  int getTick() const { return current_tick.load(); }

  void start()
  {
    spdlog::info("[Server] Starting Market Server");
    running        = true;
    listenerThread = std::thread(&MarketServer::listenForMessages, this);
    while(running)
      {
        tick();
        spdlog::info("Tick: {:<10} Orders: {:<10}", current_tick.load(), orderBook.totalOrders());
      }
  }

  void stop()
  {
    if(running)
      {
        running = false;
        pullSocket.close();
        pubSocket.close();
        if(listenerThread.joinable()) listenerThread.join();
      }
  }

  void tick()
  {
    current_tick++;
    orderBook.match();
    publishOrderBook();
  }

  private:
  void listenForMessages()
  {
    spdlog::info("[server] Starting listener thread");

    while(running)
      {
        zmq::message_t message;
        if(pullSocket.recv(message, zmq::recv_flags::none))
          {
            std::string receivedMessage(static_cast<char *>(message.data()), message.size());
            spdlog::debug("[Server] Received message: {} ", receivedMessage);
            processMessage(receivedMessage);
          }
        else { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
      }
  }

  void createMetrics(int userId)
  {
    std::unique_lock lock(metrics_mtx);
    if(metrics.find(userId) == metrics.end()) { metrics[userId] = std::make_unique<Metrics>(); }
  }

  void processMessage(const std::string &message)
  {
    try
      {
        nlohmann::json parsed = nlohmann::json::parse(message);
        if(parsed.contains("type") && (parsed["type"] == "BUY" || parsed["type"] == "SELL"))
          {
            auto newOrder = Order::fromJson(parsed);
            if(newOrder)
              {
                {
                  std::unique_lock lock(orderBook_mtx);
                  orderBook.addOrder(std::move(newOrder));
                }
              }
          }
        else { spdlog::warn(" {} Unknown message type: {}", current_tick.load(), message); }
      }
    catch(const nlohmann::json::exception &e)
      {
        spdlog::error("{} Failed to process message: {} - {}", current_tick.load(), e.what(),
                      message);
      }
  }

  void publishOrderBook()
  {
    nlohmann::json orderBookJson;
    {
      std::shared_lock lock(orderBook_mtx);
      orderBookJson = orderBook.isEmpty() ? nlohmann::json::array() : orderBook.toJson();
    }
    std::string orderBookString = orderBookJson.dump();

    zmq::message_t message(orderBookString.size());
    memcpy(message.data(), orderBookString.data(), orderBookString.size());
    // spdlog::debug("[Server] Publishing order book update: {}", orderBookString);
    pubSocket.send(message, zmq::send_flags::none);
  }

  zmq::context_t                                    context;
  zmq::socket_t                                     pullSocket;
  zmq::socket_t                                     pubSocket;
  std::atomic<bool>                                 running;
  std::thread                                       listenerThread;
  std::shared_mutex                                 metrics_mtx;
  std::shared_mutex                                 orderBook_mtx;
  std::atomic<int>                                  current_tick = 0;
  std::unordered_map<int, std::unique_ptr<Metrics>> metrics;
  OrderBook                                         orderBook;
};
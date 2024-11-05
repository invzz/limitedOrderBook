#include <zmq.hpp>
#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>
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

  int getTick() { return current_tick; }

  void start()
  {
    spdlog::info("[Server] Starting Market Server");
    running      = true;
    serverThread = std::thread(&MarketServer::listenForMessages, this);
  }

  void stop()
  {
    if(running)
      {
        running = false;
        pullSocket.close();
        pubSocket.close();
        if(serverThread.joinable()) { serverThread.join(); }
      }
  }

  void tick()
  {
    // Increment the tick and match orders
    current_tick++;
    std::lock_guard<std::mutex> lock(mtx);
    orderBook.match(current_tick, metrics);

    // Publish the order book after processing the tick
    publishOrderBook();
  }

  private:
  void listenForMessages()
  {
    spdlog::info("[Server] Starting listener thread");
    while(running)
      {
        zmq::message_t message;
        if(pullSocket.recv(message, zmq::recv_flags::dontwait))
          {
            std::string receivedMessage(static_cast<char *>(message.data()), message.size());
            processMessage(receivedMessage);
          }
      }
  }

  void processMessage(const std::string &message)
  {
    try
      {
        // Attempt to parse the message as JSON
        nlohmann::json parsed = nlohmann::json::parse(message);

        // Check if it's an order
        if(parsed.contains("type") && (parsed["type"] == "BUY" || parsed["type"] == "SELL"))
          {
            // Create an Order object from the JSON message
            auto newOrder = createOrderFromJson(parsed);
            if(newOrder)
              {
                std::lock_guard<std::mutex> lock(mtx);
                if(metrics.find(newOrder->getUserId()) == metrics.end())
                  {
                    metrics[newOrder->getUserId()] = new Metrics();
                  }
                orderBook.addOrder(std::move(newOrder));
                spdlog::info("[ time : {:>10}] + {}", current_tick, message);
              }
          }

        else { spdlog::warn(" {} Unknown message type: {}", current_tick, message); }
      }
    catch(const nlohmann::json::exception &e)
      {
        spdlog::error("{} Failed to process message: {} - {}", current_tick, e.what(), message);
      }
  }

  std::unique_ptr<Order> createOrderFromJson(const nlohmann::json &parsed)
  {
    OrderType type     = parsed["type"] == "BUY"    ? OrderType::BUY
                         : parsed["type"] == "SELL" ? OrderType::SELL
                                                    : OrderType::UNKNOWN;
    double    price    = parsed["price"];
    int       quantity = parsed["quantity"];
    int       userId   = parsed["userId"];
    return std::make_unique<Order>(type, price, quantity, userId);
  }

  void publishOrderBook()
  {
    nlohmann::json orderBookJson;
    // Check if the order book is empty
    if(orderBook.isEmpty())
      {
        // Publish an empty array
        orderBookJson = nlohmann::json::array(); // Creates an empty JSON array
      }
    else
      {
        // Convert the order book to JSON
        orderBookJson = orderBook.toJson();
      }
    std::string orderBookString = orderBookJson.dump(); // Serialize to string

    zmq::message_t message(orderBookString.size());
    memcpy(message.data(), orderBookString.data(), orderBookString.size());
    pubSocket.send(message, zmq::send_flags::none);
  }

  zmq::context_t                     context;
  zmq::socket_t                      pullSocket; // For receiving orders from bots
  zmq::socket_t                      pubSocket;  // For publishing order book updates
  std::atomic<bool>                  running;
  std::thread                        serverThread;
  std::mutex                         mtx;
  int                                current_tick = 0;
  std::unordered_map<int, Metrics *> metrics;
  OrderBook                          orderBook;
};

#include <chrono>

int main()
{
  // Set the logging level
  spdlog::set_level(spdlog::level::debug);

  // Create an instance of the MarketServer
  MarketServer server;

  // Start the server
  server.start();

  // Main loop to tick the server
  while(true)
    {
      spdlog::set_pattern("[ Market ] : %v");
      // Simulate a tick in the server
      server.tick();

      // Sleep for a short duration to simulate time between ticks
      //std::this_thread::sleep_for(std::chrono::milliseconds());
    }

  // Stop the server and cleanup
  server.stop();

  return 0;
}
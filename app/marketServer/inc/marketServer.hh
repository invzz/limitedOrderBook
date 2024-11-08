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

#define PULL_ADDRESS "tcp://localhost:5555"
#define PUB_ADDRESS  "tcp://localhost:5556"

class MarketServer
{
  public:
  MarketServer() : context(1), pullSocket(context, ZMQ_PULL), pubSocket(context, ZMQ_PUB), running(false)
  {
    pullSocket.bind(PULL_ADDRESS); // For receiving orders
    pubSocket.bind(PUB_ADDRESS);   // For publishing order book updates
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
  }

  void stop()
  {
    if(running)
      {
        running = false;
        pullSocket.close();
        pubSocket.close();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if(listenerThread.joinable()) listenerThread.join();
      }
    generateReport();
  }

  void tick()
  {
    current_tick++;

    auto trades = orderBook.match(current_tick.load());

    auto format = "[ {} ] [ {:2} --> {:2} ] [ {:2} * {:>3.2f} ]";

    for(auto &trade : trades)
      {
        spdlog::info(format, trade->getTick(), trade->getSellerId(), trade->getBuyerId(), trade->getQuantity(), trade->getPrice());
        metrics[trade->getBuyerId()]->addBuyTrade(trade);
        metrics[trade->getSellerId()]->addSellTrade(trade);
      }

    publishOrderBook();
  }

  private:
  void listenForMessages()
  {
    spdlog::info("[server] Starting listener thread");

    while(running)
      {
        zmq::message_t message;
        try
          {
            if(pullSocket.recv(message, zmq::recv_flags::dontwait))
              {
                std::string receivedMessage(static_cast<char *>(message.data()), message.size());
                spdlog::debug("[Server] Received message: {} ", receivedMessage);
                processMessage(receivedMessage);
              }
            else { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
          }
        catch(const zmq::error_t &e)
          {
            spdlog::error("[Server] Error receiving message: {}", e.what());
          }
      }
  }

  void createMetrics(std::string userId)
  {
    std::scoped_lock lock(metrics_mtx);
    if(metrics.find(userId) == metrics.end()) { metrics[userId] = std::make_unique<Metrics>(); }
  }

  void processMessage(const std::string &message)
  {
    int POSITION_LIMIT = 2000;
    try
      {
        nlohmann::json parsed = nlohmann::json::parse(message);
        if(parsed.contains("type") && (parsed["type"] == "BUY" || parsed["type"] == "SELL"))
          {
            auto newOrder = Order::fromJson(parsed);
            if(newOrder)
              {
                std::string userId = newOrder->getUserId();
                createMetrics(userId);

                // validate the order
                if(newOrder->getQuantity() <= 0)
                  {
                    spdlog::warn("[Server] Order rejected: quantity {} cannot be negative.", newOrder->getPrice(), newOrder->getQuantity());
                    return;
                  }

                if(newOrder->getPrice() <= 0)
                  {
                    spdlog::warn("[Server] Order rejected: price {} cannot be negative.", newOrder->getPrice(), newOrder->getQuantity());
                    return;
                  }

                // // for seller bots, set the user id to a negative value
                // if(newOrder->getUserId() < 0) { POSITION_LIMIT = 30000; }

                // check if the user is within the position limit
                int  quantity = newOrder->getQuantity();
                bool isBuy    = (newOrder->getType() == OrderType::BUY);
                bool isSell   = (newOrder->getType() == OrderType::SELL);

                int qtyCheck = isBuy ? quantity : isSell ? -quantity : 0;

                {
                  std::shared_lock lock(metrics_mtx);
                  if(!metrics[userId]->isWithinLimit(qtyCheck, POSITION_LIMIT))
                    {
                      spdlog::warn("[Server] Order rejected: User {} exceeds position limit with quantity {}", userId, quantity);
                      return;
                    }
                }

                // add the order to the order book
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
        spdlog::error("{} Failed to process message: {} - {}", current_tick.load(), e.what(), message);
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
    try
      {
        pubSocket.send(message, zmq::send_flags::none);
      }
    catch(const zmq::error_t &e)
      {
        spdlog::error("[Server] Error publishing order book: {}", e.what());
      }
  }

  void generateReport()
  {
    // Create a JSON object to hold the entire report
    nlohmann::json report;

    // Iterate over each bot's metrics
    for(const auto &[userId, botMetrics] : metrics)
      {
        // Collect data for each bot
        report["bots"][userId] = {
          {"totalProfit", botMetrics->getProfit()           },
          {"position",    botMetrics->getPosition()         },
          {"buyTrades",   botMetrics->getBuyTrades().size() },
          {"sellTrades",  botMetrics->getSellTrades().size()}
          // ,
          // {"buyTradesDetails",  getTradeDetails(botMetrics->getBuyTrades()) },
          // {"sellTradesDetails", getTradeDetails(botMetrics->getSellTrades())}
        };
      }

    // Optional: Add other market stats like total trades, average price, etc.
    // report["marketOverview"] = generateMarketOverview();

    // Log or print the generated report (pretty print with 4 spaces)
    spdlog::info("[Server] Simulation Report: \n{}", report.dump(4)); // Pretty print JSON with indentation
  }

  zmq::context_t                                            context;
  zmq::socket_t                                             pullSocket;
  zmq::socket_t                                             pubSocket;
  std::atomic<bool>                                         running;
  std::thread                                               listenerThread;
  std::shared_mutex                                         metrics_mtx;
  std::shared_mutex                                         orderBook_mtx;
  std::atomic<int>                                          current_tick = 0;
  std::unordered_map<std::string, std::unique_ptr<Metrics>> metrics;
  OrderBook                                                 orderBook;
};
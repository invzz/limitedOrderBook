#pragma once
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
#include "common_topics.hh" // Include the shared header file

#define PUB_ADDRESS    "tcp://*:5555"
#define PULL_ADDRESS   "tcp://*:5556"
#define ROUTER_ADDRESS "tcp://*:5557"
#define TRADE_FORMAT   "[ {} ] Trade: {} -> {} [ {} @ {} ]"

class MarketServer
{
  public:
  MarketServer();
  ~MarketServer();

  int getTick() const;

  void start();
  void stop();
  void liquidatePositions();
  void generateReport();
  void ListenForCommands();
  void PutOrder(const std::string &message);
  void GetMetrics(const std::string &clientId);

  private:
  void publishOrderBook();


  std::atomic<bool> running;
  std::atomic<int>  current_tick = 0;

  zmq::context_t context;
  zmq::socket_t  pubSocket;
  zmq::socket_t  routerSocket; // Add a ROUTER socket

  std::thread                                               OrderListenerThread;
  std::thread                                               CommandListenerThread;
  std::shared_mutex                                         metrics_mtx;
  std::shared_mutex                                         orderBook_mtx;
  std::unordered_map<std::string, std::unique_ptr<Metrics>> metrics;
  OrderBook                                                 orderBook;

  std::atomic<double> lastAvgPrice = 0.0;
};

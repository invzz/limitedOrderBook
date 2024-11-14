#include "marketServer.hh"
#include "commands/commandFactory.hh"
#include <chrono>

MarketServer::MarketServer() : running(false), context(1), pubSocket(context, ZMQ_PUB), routerSocket(context, ZMQ_ROUTER)
{
  pubSocket.bind(PUB_ADDRESS);       // 5555
  routerSocket.bind(ROUTER_ADDRESS); // 5557
}

MarketServer::~MarketServer() { stop(); }

int MarketServer::getTick() const { return current_tick.load(); }

void MarketServer::start()
{
  running               = true;
  CommandListenerThread = std::thread(&MarketServer::ListenForCommands, this);

  while(running)
    {
      current_tick++;
      auto trades = orderBook.match(current_tick.load());
      if(!orderBook.isEmpty()) { lastAvgPrice = orderBook.getAvgPrice(); }
      for(auto &trade : trades)
        {
          if(running)
            {
              spdlog::info(TRADE_FORMAT, trade->getTick(), trade->getSellerId(), trade->getBuyerId(), trade->getQuantity(), trade->getPrice());
              metrics[trade->getBuyerId()]->addBuyTrade(trade);
              metrics[trade->getSellerId()]->addSellTrade(trade);
            }
        }
      publishOrderBook();
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void MarketServer::stop()
{
  if(running)
    {
      running = false;
      pubSocket.close();
      routerSocket.close();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      if(OrderListenerThread.joinable()) OrderListenerThread.join();
      liquidatePositions();
      generateReport();
    }
}

void MarketServer::liquidatePositions()
{
  std::scoped_lock lock(metrics_mtx);
  {
    std::shared_lock lock(orderBook_mtx);
    auto             bestBid = orderBook.getBestBid();
    auto             bestAsk = orderBook.getBestAsk();
  }
  for(const auto &[userId, botMetrics] : metrics)
    {
      int position = botMetrics->getPosition();
      spdlog::info("[Server] Liquidating position {} for user: {} , adding {} to : {} ", position, userId, position * lastAvgPrice, botMetrics->getProfit());
      botMetrics->updateProfit(position * lastAvgPrice);
    }
}

void MarketServer::generateReport()
{
  nlohmann::json report;
  for(const auto &[userId, botMetrics] : metrics)
    {
      report["bots"][userId] = {
        {"totalProfit", botMetrics->getProfit()           },
        {"position",    botMetrics->getPosition()         },
        {"buyTrades",   botMetrics->getBuyTrades().size() },
        {"sellTrades",  botMetrics->getSellTrades().size()}
      };
    }

  spdlog::info("[Server] Simulation Report: \n{}", report.dump(4));
}

void MarketServer::ListenForCommands()
{
  zmq::message_t request;
  while(running)
    {
      if(routerSocket.recv(request, zmq::recv_flags::none) && routerSocket.recv(request, zmq::recv_flags::none))
        {
          std::string        message(static_cast<char *>(request.data()), request.size());
          std::string        body;
          std::istringstream iss(message);
          std::string        command;
          std::string        userId;

          iss >> command >> userId >> body;

          auto cmd = CommandFactory::createCommand(command, userId, this);

          if(cmd) { cmd->execute(body); }

          if(command == "STOP") break;
        }
    }
}

void MarketServer::PutOrder(const std::string &message)
{
  if(!running) { return; }

  int POSITION_LIMIT = 200;

  try
    {
      nlohmann::json parsed = nlohmann::json::parse(message);
      if(parsed.contains("type") && (parsed["type"] == "BUY" || parsed["type"] == "SELL"))
        {
          auto newOrder = Order::fromJson(parsed);
          int  quantity = newOrder->getQuantity();
          if(newOrder)
            {
              std::string userId = newOrder->getUserId();

              // validate the order
              if(quantity <= 0)
                {
                  spdlog::debug("[Server] Order rejected: quantity {} cannot be negative.", newOrder->getPrice(), quantity);
                  return;
                }

              if(quantity <= 0)
                {
                  spdlog::debug("[Server] Order rejected: price {} cannot be negative.", newOrder->getPrice(), quantity);
                  return;
                }

              bool isBuy  = (newOrder->getType() == OrderType::BUY);
              bool isSell = (newOrder->getType() == OrderType::SELL);

              int qtyCheck = isBuy ? quantity : isSell ? -quantity : 0;

              if(metrics.find(userId) == metrics.end())
                {
                  std::unique_lock lock(metrics_mtx);

                  metrics[userId] = std::make_unique<Metrics>();
                }

              {
                std::unique_lock lock(metrics_mtx);
                if(!metrics[userId]->isWithinLimit(qtyCheck, POSITION_LIMIT))
                  {
                    spdlog::debug("[Server] Order rejected: User {} exceeds position limit with quantity {}", userId, quantity);
                    return;
                  }
              }

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

void MarketServer::publishOrderBook()
{
  nlohmann::json orderBookJson;
  {
    std::shared_lock lock(orderBook_mtx);
    orderBookJson = orderBook.isEmpty() ? nlohmann::json::array() : orderBook.toJson();
  }

  std::string orderBookString = orderBookJson.dump();

  zmq::message_t topic(BOOK_TOPIC, strlen(BOOK_TOPIC));
  zmq::message_t message(orderBookString.size());

  memcpy(message.data(), orderBookString.data(), orderBookString.size());

  pubSocket.send(topic, zmq::send_flags::sndmore);
  pubSocket.send(message, zmq::send_flags::none);

  auto totalsize = orderBook.getBestAsk().size() + orderBook.getBestBid().size();
  auto avgPrice  = orderBook.getAvgPrice();

  spdlog::info("[Server] Published order book of {} elements. with avgPrice {}", totalsize, avgPrice);
}

void MarketServer::GetMetrics(const std::string &userId)
{
  {
    std::string metricsString;
    {
      std::unique_lock lock(metrics_mtx);
      if(metrics[userId] == nullptr) metrics[userId] = std::make_unique<Metrics>();
      metricsString = metrics[userId]->toJson().dump();
    }
    zmq::message_t client(userId.data(), userId.size());
    zmq::message_t message(metricsString.size());
    
    memcpy(message.data(), metricsString.data(), metricsString.size());
    
    routerSocket.send(client, zmq::send_flags::sndmore);
    routerSocket.send(message, zmq::send_flags::none);
    spdlog::info("[Server] Sent metrics to client: {}", userId);
  }
}
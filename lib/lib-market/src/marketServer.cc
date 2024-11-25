#include "marketServer.hh"

namespace market
{
    const std::string PRODUCT = "product";
    // clang-format off
MarketServer::MarketServer()
    : orderBookService_(std::make_shared<OrderBookService>(PRODUCT)), 
      mediator_(std::make_shared<MarketMediator<MarketServer>>()),
      tradeTrackerService_(std::make_shared<TradeTrackerService>()), 
      context_(1),
      pubSocket_(context_, zmq::socket_type::pub), 
      routerSocket_(context_, zmq::socket_type::router), 
      running_(true), 
      current_tick_(0)
    // clang-format on
    {
        pubSocket_.bind(SERVER_PUB_ADDRESS);

        routerSocket_.set(zmq::sockopt::router_mandatory, true);
        routerSocket_.set(zmq::sockopt::rcvtimeo, ROUTER_SOCKET_TIMEOUT);
        routerSocket_.bind(SERVER_ROUTER_ADDRESS);
    }

    void MarketServer::initialize()
    {
        controller_ = std::make_shared<MarketController<MarketServer>>(orderBookService_, tradeTrackerService_, shared_from_this());
        mediator_->setController(controller_);
    }

    void MarketServer::mainLoop()

    {
        spdlog::info("[Server] Starting main loop");
        while(running_)
            {
                current_tick_++;

                mediator_->handleRequest("MATCH_ORDERS", current_tick_);

                if(!running_) { break; }

                publishOrderBook();

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_LOOP_SLEEP_TIME));
        pubSocket_.close();
        spdlog::info("[Server] Stopping main loop");
    }

    void MarketServer::start()
    {
        CommandListenerThread_ = std::thread(&MarketServer::commandLoop, this);
        MainLoopThread_        = std::thread(&MarketServer::mainLoop, this);
    }

    void MarketServer::stop()
    {
        spdlog::warn("[Server] Stopping server");
        running_ = false;

        if(CommandListenerThread_.joinable())
            {
                spdlog::debug("[Server] Joining CommandListenerThread");
                CommandListenerThread_.join();
            }

        if(MainLoopThread_.joinable())
            {
                spdlog::debug("[Server] Joining MainLoopThread");
                MainLoopThread_.join();
            }

        pubSocket_.close();
        routerSocket_.close();
        context_.close();

        spdlog::info("[Server] Server stopped");

        generateReport();
    }

    void MarketServer::sendMessage(const std::string &userId, const std::string &content)
    {
        zmq::message_t message(userId.size());
        zmq::message_t body(content.size());

        std::memcpy(body.data(), content.data(), content.size());
        std::memcpy(message.data(), userId.data(), userId.size());

        routerSocket_.send(message, zmq::send_flags::sndmore);
        routerSocket_.send(zmq::message_t(), zmq::send_flags::sndmore);
        routerSocket_.send(body, zmq::send_flags::none);
    }

    void MarketServer::commandLoop()
    {
        spdlog::info("[Server] Starting command loop");
        zmq::message_t request;

        while(running_)
            {
                try
                    {
                        zmq::recv_result_t result = routerSocket_.recv(request, zmq::recv_flags::dontwait);
                        if(result && routerSocket_.recv(request, zmq::recv_flags::dontwait))
                            {
                                std::string        message(static_cast<char *>(request.data()), request.size());
                                std::string        body;
                                std::istringstream iss(message);
                                std::string        command;
                                std::string        userId;

                                iss >> command >> userId >> body;

                                mediator_->handleRequest(command, body, userId);

                                spdlog::debug("[ {} ] Received command: {} from user: {}", __func__, command, userId);
                            }
                        // Check the running_ flag
                        if(!running_) { break; }
                    }
                catch(const zmq::error_t &e)
                    {
                        if(e.num() != EAGAIN) {}
                    }
                if(!running_) { break; }
            }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        routerSocket_.close();
        spdlog::info("[Server] Stopping command loop");
    }

    void MarketServer::publishOrderBook()
    {
        nlohmann::json orderBookJson   = orderBookService_->getAsJson();
        std::string    orderBookString = orderBookJson.dump();

        zmq::message_t topic(BOOK_TOPIC, strlen(BOOK_TOPIC));
        zmq::message_t message(orderBookString.size());

        memcpy(message.data(), orderBookString.data(), orderBookString.size());

        pubSocket_.send(topic, zmq::send_flags::sndmore);
        pubSocket_.send(message, zmq::send_flags::none);

        double avgPrice     = orderBookService_->getAvgPrice();
        size_t orders_count = orderBookService_->getTotalOrdersCount();

        spdlog::debug("[Server] Published order book of {} elements. with avgPrice {}", orders_count, avgPrice);
    }

    void MarketServer::liquidatePositions()
    {
        std::vector<std::string> ids = tradeTrackerService_->getIds();

        for(const auto &id : ids)
            {
                auto service  = tradeTrackerService_->getTradeService(id);
                auto position = service->getPosition();
                spdlog::info("[Server] Liquidating position for user: {}", id);
                auto money = position * orderBookService_->getAvgPrice();
                service->liquidate(current_tick_, money, id);
            }
    }

    void MarketServer::generateReport()
    {
        auto report = tradeTrackerService_->getAsJson(false);

        report["last tick"]   = current_tick_;
        report["totalOrders"] = orderBookService_->getTotalOrdersCount();
        report["avgPrice"]    = orderBookService_->getAvgPrice();

        spdlog::info("[Server] Simulation Report: \n{}", report.dump(4));
    }
} // namespace market
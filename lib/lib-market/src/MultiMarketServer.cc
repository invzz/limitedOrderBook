#include "MultiMarketServer.hh"
#include "custom_formatter.hh"
#include <future>

namespace market
{

    MultiMarketServer::MultiMarketServer()
        : orderBookService_(std::make_shared<MultiOrderBookService>()), tradeTrackerService_(std::make_shared<MultiTradeTrackerService>()), router_(std::make_shared<MarketServiceRouter<MultiMarketServer>>()), context_(1),
          pubSocket_(context_, zmq::socket_type::pub), routerSocket_(context_, zmq::socket_type::router), running_(true), current_tick_(0)
    {
        pubSocket_.bind(SERVER_PUB_ADDRESS);

        routerSocket_.set(zmq::sockopt::router_mandatory, true);
        routerSocket_.set(zmq::sockopt::rcvtimeo, ROUTER_SOCKET_TIMEOUT);
        routerSocket_.bind(SERVER_ROUTER_ADDRESS);
    }

    void MultiMarketServer::initialize() { router_->setServer(shared_from_this()); }

    void MultiMarketServer::mainLoop()
    {
        spdlog::info("[ {} ] Starting main loop", market::yellow("Server"));
        while(running_)
            {
                current_tick_++;
                // Parallelize the loop using std::async
                auto                           products = orderBookService_->getProducts();
                
                std::vector<std::future<void>> futures;
                
                for(auto &product : products)
                    {
                        futures.push_back(std::async(std::launch::async, [this, &product]() {
                            auto mediator = router_->getOrCreate(product);
                            mediator->handleRequest("MATCH_ORDERS", current_tick_, "admin");
                        }));
                    }

                // Wait for all futures to complete
                for(auto &future : futures) { future.get(); }

                publishOrderBook();

                std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_LOOP_SLEEP_TIME));
            }

        spdlog::info("[ {} ] Stopping main loop", market::yellow("Server"));
    }

    void MultiMarketServer::start()
    {
        CommandListenerThread_ = std::thread(&MultiMarketServer::commandLoop, this);
        MainLoopThread_        = std::thread(&MultiMarketServer::mainLoop, this);
    }

    void MultiMarketServer::stop()
    {
        spdlog::warn("[ {} ] Stopping server", market::yellow("Server"));
        running_ = false;

        if(CommandListenerThread_.joinable())
            {
                spdlog::debug("[ {} ] Terminating CommandListenerThread", market::yellow("Server"));
                CommandListenerThread_.join();
            }

        if(MainLoopThread_.joinable())
            {
                spdlog::debug("[ {} ] Terminating MainLoopThread", market::yellow("Server"));
                MainLoopThread_.join();
            }

        pubSocket_.close();
        routerSocket_.close();
        context_.close();

        spdlog::info("[ {} ] Server stopped", market::yellow("Server"));

        generateReport();
    }

    void MultiMarketServer::sendMessage(const std::string &userId, const std::string &content)
    {
        zmq::message_t message(userId.size());
        zmq::message_t body(content.size());

        std::memcpy(message.data(), userId.data(), userId.size());
        std::memcpy(body.data(), content.data(), content.size());

        routerSocket_.send(message, zmq::send_flags::sndmore);
        routerSocket_.send(zmq::message_t(), zmq::send_flags::sndmore);
        routerSocket_.send(body, zmq::send_flags::none);
    }

    void MultiMarketServer::commandLoop()
    {
        spdlog::info("[ {} ] Starting command loop", market::yellow("Server"));
        zmq::message_t request;

        while(running_)
            {
                try
                    {
                        zmq::recv_result_t result = routerSocket_.recv(request, zmq::recv_flags::dontwait);
                        if(result && routerSocket_.recv(request, zmq::recv_flags::dontwait))
                            {
                                std::string message(static_cast<char *>(request.data()), request.size());

                                std::string        userId;
                                std::string        body;
                                std::istringstream iss(message);
                                std::string        command;
                                iss >> command >> userId >> body;

                                router_->handleCommand(command, body, userId);

                                spdlog::debug("[ {} ] Received command: {} from user: {}", market::blue(__func__), market::green(command), userId);
                            }
                    }
                catch(const zmq::error_t &e)
                    {
                        if(e.num() != EAGAIN) { spdlog::error("[Server] ZMQ error: {}", e.what()); }
                    }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

        spdlog::info("[ {} ] Stopping command loop", market::yellow("Server"));
    }

    void MultiMarketServer::publishOrderBook()
    {
        nlohmann::json combinedOrderBook;

        for(const auto &product : orderBookService_->getProducts()) { combinedOrderBook[product] = orderBookService_->getOrCreate(product)->getAsJson(); }

        std::string combinedOrderBookString = combinedOrderBook.dump();

        zmq::message_t topic(BOOK_TOPIC, strlen(BOOK_TOPIC));
        zmq::message_t message(combinedOrderBookString.size());

        std::memcpy(message.data(), combinedOrderBookString.data(), combinedOrderBookString.size());

        pubSocket_.send(topic, zmq::send_flags::sndmore);
        pubSocket_.send(message, zmq::send_flags::none);

        spdlog::debug("[ {} ] Published combined order book for {} products", market::yellow("Server"), combinedOrderBook.size());
    }

    void MultiMarketServer::liquidatePositions()
    {
        // for(const auto &product : tradeTrackerService_->getProducts())
        //     {
        //         std::vector<std::string> ids = tradeTrackerService_->getOrCreate(product)->getIds();

        //         for(const auto &id : ids)
        //             {
        //                 auto service  = tradeTrackerService_->getOrCreate(product)->getTradeService(id);
        //                 auto position = service->getPosition();
        //                 spdlog::info("[Server] Liquidating position for user: {} on product: {}", id, product);
        //                 auto money = position * orderBookService_->getOrCreate(product)->getAvgPrice();
        //                 service->liquidate(current_tick_, money, id);
        //             }
        //     }
    }

    void MultiMarketServer::generateReport()
    {
        // nlohmann::json report;

        // for(const auto &product : tradeTrackerService_->getProducts()) { report[product] = tradeTrackerService_->getOrCreate(product)->getAsJson(false); }

        // report["last_tick"]    = current_tick_;
        // report["total_orders"] = orderBookService_->getTotalOrdersCount();
        // report["avg_price"]    = orderBookService_->getAvgPrice();

        // spdlog::info("[Server] Simulation Report:\n{}", report.dump(4));
    }

} // namespace market

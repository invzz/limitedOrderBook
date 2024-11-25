#pragma once
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include "MultiOrderBookService.hh"
#include "common.hh"

namespace market
{
    class MultiMarketClient
    {
        public:
        MultiMarketClient(std::string userId) : context(1), subSocket(context, zmq::socket_type::sub), dealerSocket(context, zmq::socket_type::dealer), userId(userId)
        {
            orderBookServices_ = std::make_shared<MultiOrderBookService>();
            spdlog::info("[ {} ] Connecting to [ {} ] - Subscribing to topic: {}", userId, CLIENT_PULL_ADDRESS, BOOK_TOPIC);
            subSocket.connect(CLIENT_PULL_ADDRESS);
            subSocket.set(zmq::sockopt::subscribe, BOOK_TOPIC);
            dealerSocket.set(zmq::sockopt::routing_id, userId);
            spdlog::info("[ {} ] Connecting to [ {} ] - dealer => router", userId, CLIENT_ROUTER_ADDRESS);
            dealerSocket.connect(CLIENT_ROUTER_ADDRESS);
        }

        std::shared_ptr<MultiOrderBookService> getOrderBookServices() const { return orderBookServices_; }
        std::string                            getUserId() const { return userId; }

        virtual void run() = 0;

        void doRun() { run(); }

        void start()
        {
            spdlog::info("Starting Bot {}", userId);
            running   = true;
            subThread = std::thread(&MultiMarketClient::listenForOrderBookSub, this);
        }

        void stop()
        {
            spdlog::info("[ {} ] Stopping...", userId);
            running = false;
            if(subThread.joinable()) { subThread.join(); }
            if(dealerThread.joinable()) { dealerThread.join(); }
        }

        void putOrder(const std::string &product, const nlohmann::json &order)
        {
            nlohmann::json productOrder;

            productOrder["product"] = product;
            productOrder["order"]   = order;

            std::string    message = "PUT_ORDER " + userId + " " + productOrder.dump();
            zmq::message_t request(message.c_str(), message.size());

            dealerSocket.send(request, zmq::send_flags::dontwait);

            spdlog::debug("[ {} ] Sent order:\n{}", userId, order.dump(4));
            getMetrics();
        }

        private:
        std::shared_ptr<MultiOrderBookService> orderBookServices_;
        std::atomic<bool>                      running{true};
        zmq::context_t                         context;
        zmq::socket_t                          subSocket;
        zmq::socket_t                          dealerSocket;
        std::thread                            subThread;
        std::thread                            dealerThread;
        std::string                            userId;

        void getMetrics()
        {
            // std::string    message = "GET_METRICS " + userId;
            // zmq::message_t request(message.c_str(), message.size());
            // dealerSocket.send(request, zmq::send_flags::dontwait);
            // spdlog::debug("[ {} ] Requested metrics", userId);
            // listenForMetrics();
        }

        void updateOrderBook(const std::string &update)
        {
            try
                {
                    nlohmann::json orderBookUpdate = nlohmann::json::parse(update);

                    if(orderBookUpdate.is_array() && orderBookUpdate.empty())
                        {
                            spdlog::debug("[ {} ] order book is empty", userId);
                            orderBookServices_->clear();
                        }
                    else
                        {
                            orderBookServices_->updateOrderBook(orderBookUpdate, true);
                            spdlog::debug("[ {} ] Updated order book", userId);
                        }
                }
            catch(const std::exception &e)
                {
                    orderBookServices_->clear();
                    spdlog::error("[ {} ] Failed to update order book: {}", userId, e.what());
                }
        }

        void listenForMetrics()
        {
            zmq::message_t identity;
            zmq::message_t message;

            if(dealerSocket.recv(identity, zmq::recv_flags::dontwait))
                {
                    if(dealerSocket.recv(message, zmq::recv_flags::dontwait))
                        {
                            std::string receivedMessage(static_cast<char *>(message.data()), message.size());
                            updateMetrics(receivedMessage);
                        }
                }
        }
        void updateMetrics(const std::string &update)
        {
            try
                {
                    nlohmann::json metricsUpdate = nlohmann::json::parse(update);
                }
            catch(const nlohmann::json::exception &e)
                {
                    spdlog::error("[ {} ] Failed to process metrics update: {} - {}", userId, e.what(), update);
                }
        }
        void listenForOrderBookSub()
        {
            while(running)
                {
                    zmq::message_t topic;
                    zmq::message_t message;
                    if(subSocket.recv(topic, zmq::recv_flags::dontwait) && subSocket.recv(message, zmq::recv_flags::dontwait))
                        {
                            std::string receivedMessage(static_cast<char *>(message.data()), message.size());
                            spdlog::debug("[ {} ] Received message: {}", userId, receivedMessage);
                            updateOrderBook(receivedMessage);
                            doRun();
                        }
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            spdlog::info("[ {} ] terminating thread {} ", userId, __func__);
        }
    };
} // namespace market
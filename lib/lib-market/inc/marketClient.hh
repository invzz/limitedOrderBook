#pragma once
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include "orderBookService.hh"
#include "common.hh"

class MarketClient
{
    public:
    MarketClient(const std::string &serverAddress, std::string userId)
        : context(1), subSocket(context, zmq::socket_type::sub), dealerSocket(context, zmq::socket_type::dealer), userId(userId)
    {
        orderBookService = std::make_shared<OrderBookService>();
        subSocket.connect(CLIENT_PULL_ADDRESS);
        subSocket.set(zmq::sockopt::subscribe, BOOK_TOPIC);
        dealerSocket.set(zmq::sockopt::routing_id, userId);
        dealerSocket.connect(CLIENT_ROUTER_ADDRESS);
    }

    virtual void run() = 0;

    void doRun() { run(); }

    void start()
    {
        spdlog::info("Starting Bot {}", userId);
        running   = true;
        subThread = std::thread(&MarketClient::listenForOrderBookSub, this);
    }

    void stop()
    {
        spdlog::info("[ {} ] Stopping...", userId);
        running = false;
        if(subThread.joinable()) { subThread.join(); }
        if(dealerThread.joinable()) { dealerThread.join(); }
    }
    std::string getUserId() const { return userId; }

    protected:
    std::shared_ptr<OrderBookService> orderBookService;

    void putOrder(const nlohmann::json &order)
    {
        std::string    message = "PUT_ORDER " + userId + " " + order.dump();
        zmq::message_t request(message.c_str(), message.size());
        dealerSocket.send(request, zmq::send_flags::dontwait);
        spdlog::debug("[ {} ] Sent order:\n{}", userId, order.dump(4));
        getMetrics();
    }

    void getMetrics()
    {
        std::string    message = "GET_METRICS " + userId;
        zmq::message_t request(message.c_str(), message.size());
        dealerSocket.send(request, zmq::send_flags::dontwait);
        spdlog::debug("[ {} ] Requested metrics", userId);
        listenForMetrics();
    }

    void updateOrderBook(const std::string &update)
    {
        try
            {
                nlohmann::json orderBookUpdate = nlohmann::json::parse(update);

                if(orderBookUpdate.is_array() && orderBookUpdate.empty())
                    {
                        spdlog::debug("[ {} ] order book is empty", userId);
                        orderBookService->clear();
                    }
                else
                    {
                        orderBookService->updateOrderBook(orderBookUpdate, true);
                        spdlog::debug("[ {} ] Updated order book:\n{}", userId, orderBookUpdate.dump());
                    }
            }
        catch(const std::exception &e)
            {
                spdlog::error("[ {} ] Failed to update order book: {}", userId, e.what());
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
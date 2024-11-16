
#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include "order.hh" // Assuming you have this header that defines Order and its methods
#include "nlohmann/json.hpp"

class PriceLevel
{
    private:
    double                              price;
    std::vector<std::shared_ptr<Order>> orders; // Change to shared_ptr
    mutable std::mutex                  mtx;    // Mutex to protect access to orders

    public:
    PriceLevel(double price) : price(price) {}

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
        return orders.size();
    }

    nlohmann::json getAsJson() const
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
        nlohmann::json              json;
        json["price"] = price;

        nlohmann::json ordersJson = nlohmann::json::array();
        for(const auto &order : orders)
            {
                if(order) ordersJson.push_back(order->getAsJson());
            }
        json["orders"] = ordersJson;

        return json;
    }

    static std::shared_ptr<PriceLevel> fromJson(const nlohmann::json &priceLevelData)
    {
        double price      = priceLevelData["price"];
        auto   priceLevel = std::make_shared<PriceLevel>(price); // Use shared_ptr

        // Deserialize each order within this price level
        for(const auto &orderData : priceLevelData["orders"])
            {
                auto order = Order::fromJson(orderData);
                priceLevel->orders.push_back(order); // No need for std::move with shared_ptr
            }
        return priceLevel;
    }

    void addOrder(std::shared_ptr<Order> order) // Use shared_ptr here
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
        orders.push_back(order);               // No need for move semantics
    }

    void removeOrder(Order *order)
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
        orders.erase(std::remove_if(orders.begin(), orders.end(),
                                    [order](const std::shared_ptr<Order> &ptr) {
                                        return ptr.get() == order; // Compare the raw pointer
                                    }),
                     orders.end());
    }

    void removeFilledOrders()
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
        orders.erase(std::remove_if(orders.begin(), orders.end(),
                                    [](const std::shared_ptr<Order> &ptr) {
                                        return ptr->getQuantity() == 0; // Remove filled orders
                                    }),
                     orders.end());
    }

    // Getter
    double getPrice() const { return price; }

    // Getter for orders (const method)
    std::vector<std::shared_ptr<Order>> &getOrders() { return orders; }

    // Setter
    void setPrice(double newPrice) { price = newPrice; }
    void setOrders(std::vector<std::shared_ptr<Order>> newOrders) { orders = std::move(newOrders); }
};

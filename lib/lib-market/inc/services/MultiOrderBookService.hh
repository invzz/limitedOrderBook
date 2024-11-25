
#pragma once
#include "orderBookService.hh"

namespace market
{
    class MultiOrderBookService
    {
        public:
        MultiOrderBookService() {}

        std::shared_ptr<OrderBookService> getOrCreate(const std::string &product);
        nlohmann::json                    getAsJson();

        double                   getAvgPrice(const std::string &productId);
        size_t                   getTotalOrdersCount(const std::string &productId);
        void                     updateOrderBook(const std::string &productId, const nlohmann::json &orderBookData, bool doClean = false);
        void                     updateOrderBook(nlohmann::json &orderBookData, bool doClean = false);
        void                     clear();
        std::vector<std::string> getProducts() const;

        private:
        std::unordered_map<std::string, std::shared_ptr<OrderBookService>> productOrderBooks_;
    };
} // namespace market

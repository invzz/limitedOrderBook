// trade.hh
#ifndef TRADE_HH
#define TRADE_HH

#include <string>
namespace market
{
    class Trade
    {
        public:
        Trade(int tick, std::string buyerId, std::string sellerId, double price, int quantity) : tick(tick), buyerId(buyerId), sellerId(sellerId), price(price), quantity(quantity) {}
        int         getId() const { return id; }
        int         getTick() const { return tick; }
        std::string getBuyerId() const { return buyerId; }
        std::string getSellerId() const { return sellerId; }
        double      getPrice() const { return price; }
        int         getQuantity() const { return quantity; }

        void setId(int newId) { id = newId; }

        nlohmann::json getAsJson() const
        {
            return nlohmann::json{
              {"id",       id      },
              {"tick",     tick    },
              {"buyerId",  buyerId },
              {"sellerId", sellerId},
              {"price",    price   },
              {"quantity", quantity}
            };
        }

        private:
        int         id;
        int         tick;
        std::string buyerId;
        std::string sellerId;
        double      price;
        int         quantity;
    };
} // namespace market
#endif // TRADE_HH

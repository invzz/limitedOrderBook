// trade.hh
#ifndef TRADE_HH
#define TRADE_HH

#include <string>

class Trade
{
  public:
  Trade(int tick, std::string buyerId, std::string sellerId, double price, int quantity)
      : tick(tick), buyerId(buyerId), sellerId(sellerId), price(price), quantity(quantity)
  {}
  int         getTick() const { return tick; }
  std::string getBuyerId() const { return buyerId; }
  std::string getSellerId() const { return sellerId; }
  double      getPrice() const { return price; }
  int         getQuantity() const { return quantity; }

  private:
  int         tick;
  std::string buyerId;
  std::string sellerId;
  double      price;
  int         quantity;
};

#endif // TRADE_HH

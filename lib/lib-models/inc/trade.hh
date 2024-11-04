// trade.hh
#ifndef TRADE_HH
#define TRADE_HH

#include <string>

class Trade
{
  public:
  Trade(int tick, int buyerId, int sellerId, double price, int quantity)
      : tick(tick), buyerId(buyerId), sellerId(sellerId), price(price), quantity(quantity)
  {}
  int    getTick() const { return tick; }
  int    getBuyerId() const { return buyerId; }
  int    getSellerId() const { return sellerId; }
  double getPrice() const { return price; }
  int    getQuantity() const { return quantity; }

  private:
  int    tick;
  int    buyerId;
  int    sellerId;
  double price;
  int    quantity;
};

#endif // TRADE_HH

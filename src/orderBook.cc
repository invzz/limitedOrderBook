#include "orderBook.hh"
#include "spdlog/spdlog.h"

void OrderBook::addOrder(std::unique_ptr<Order> new_order)
{
  double    price    = new_order->getPrice();
  OrderType type     = new_order->getType();
  int       id       = new_order->getId();
  int       quantity = new_order->getQuantity();

  if(id == 0) { new_order->setId(next_order_id++); }

  if(quantity <= 0)
    {
      spdlog::error("Order ID: {:03} has invalid quantity: {:03}", id, quantity);
      return;
    }

  if(type == OrderType::BUY)
    {
      // If there are no buy orders at this price, create a new PriceLevel
      if(buy_orders.find(price) == buy_orders.end())
        {
          buy_orders[price] = std::make_unique<PriceLevel>(price);
        }
      buy_orders[price]->addOrder(std::move(new_order));
    }
  else if(type == OrderType::SELL)
    {
      // If there are no sell orders at this price, create a new PriceLevel
      if(sell_orders.find(price) == sell_orders.end())
        {
          sell_orders[price] = std::make_unique<PriceLevel>(price);
        }
      sell_orders[price]->addOrder(std::move(new_order)); // Move the order into the price level
    }
}

void OrderBook::match(std::unordered_map<int, Metrics *> metricsMap)
{
  // Iterate through all buy orders
  for(auto it_buy = buy_orders.begin(); it_buy != buy_orders.end();)
    {
      double buy_price = it_buy->first;               // Get the buy price
      auto  &buyOrders = it_buy->second->getOrders(); // Get the buy orders

      // Iterate through all sell orders
      for(auto it_sell = sell_orders.begin(); it_sell != sell_orders.end();)
        {
          double sell_price = it_sell->first; // Get the sell price

          // Check if we can match the buy and sell orders
          if(canMatch(buy_price, sell_price))
            {
              for(auto &buyOrder : buyOrders)
                {
                  for(auto &sellOrder : it_sell->second->getOrders())
                    {
                      if(sellOrder == nullptr || buyOrder == nullptr) continue;
                      // Prevent self-trading
                      if(buyOrder->getUserId() == sellOrder->getUserId()) continue;

                      // Calculate the quantity to trade
                      int quantityTraded =
                        std::min(buyOrder->getQuantity(), sellOrder->getQuantity());

                      // Update quantities
                      buyOrder->updateQuantity(-quantityTraded);
                      sellOrder->updateQuantity(-quantityTraded);

                      int buyerId  = buyOrder->getId();
                      int sellerId = sellOrder->getId();

                             

                      spdlog::debug("[ OrderBook ] :: [MATCH!] :: [ BUY {:03} ] [ SELL {:03} ] : [ "
                                    "{:03} ] at price [ {:.2f} ]",
                                    buyOrder->getId(), sellOrder->getId(), quantityTraded,
                                    sell_price);

                      Trade trade(buyOrder->getUserId(), sellOrder->getUserId(), sell_price,
                                  quantityTraded);
                      
                      metricsMap[buyOrder->getUserId()]->addBuyTrade(trade);
                      metricsMap[sellOrder->getUserId()]->addSellTrade(trade);

                     
                      // Remove filled orders
                      if(sellOrder->getQuantity() == 0)
                        {
                          it_sell->second->removeOrder(sellOrder.get());
                        }

                      if(buyOrder->getQuantity() == 0)
                        {
                          it_buy->second->removeOrder(buyOrder.get());
                          break; // Break out of the inner loop if the buy order is filled
                        }
                    }
                }

              // Check if the sell level is empty after processing
              if(it_sell->second->getOrders().empty())
                {
                  it_sell = sell_orders.erase(it_sell); // Erase and get the next iterator
                }
              else
                {
                  ++it_sell; // Move to the next sell level
                }

              // Check if the buy level is empty after processing
              if(it_buy->second->getOrders().empty())
                {
                  it_buy = buy_orders.erase(it_buy); // Erase and get the next iterator
                  break; // Break out of the sell order loop since all buy orders are matched
                }
            }
          else
            {
              ++it_sell; // Move to the next sell level if no match
            }
        }

      // If no matches were made for this buy level, move to the next buy level
      if(it_buy != buy_orders.end()) { ++it_buy; }
    }
}

// Helper method to check if a buy and sell order can match
bool OrderBook::canMatch(double buy_price, double sell_price) { return buy_price >= sell_price; }

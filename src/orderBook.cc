#include "orderBook.hh"
#include "spdlog/spdlog.h"

void OrderBook::addOrder(Order *new_order)
{
  std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
  auto                       &levels =
    (new_order->getType() == OrderType::BUY) ? buy_orders : sell_orders;

  if(levels.find(new_order->getPrice()) == levels.end())
    {
      levels[new_order->getPrice()] =
        std::make_unique<PriceLevel>(new_order->getPrice());
    }

  levels[new_order->getPrice()]->addOrder(new_order);

  match();
}

void OrderBook::match()
{
  for(auto it_buy = buy_orders.begin(); it_buy != buy_orders.end();)
    {
      double buy_price = it_buy->first;
      bool   matched   = false;

      for(auto it_sell = sell_orders.begin(); it_sell != sell_orders.end();)
        {
          double sell_price = it_sell->first;

          // Check if a match can occur
          if(canMatch(buy_price, sell_price))
            {
              matched = true;
              matchBuySellOrders(it_buy->second.get(), it_sell->second.get());

              // Remove sell level if empty
              if(it_sell->second->getOrders().empty())
                {
                  it_sell = sell_orders.erase(it_sell);
                }
              else
                {
                  ++it_sell; // Move to the next sell level
                }

              // Remove buy level if all orders are matched
              if(it_buy->second->getOrders().empty())
                {
                  it_buy = buy_orders.erase(it_buy);
                  break; // Break out of the sell order loop since all buy
                         // orders are matched
                }
            }
          else
            {
              ++it_sell; // Move to the next sell level if no match
            }
        }

      // If no matches were made for this buy level, move to the next buy level
      if(!matched) { ++it_buy; }
    }
}

// Helper method to check if a buy and sell order can match
bool OrderBook::canMatch(double buy_price, double sell_price)
{
  return buy_price >= sell_price;
}

// Helper method to match buy and sell orders within the given price levels
void OrderBook::matchBuySellOrders(PriceLevel *buy_level,
                                   PriceLevel *sell_level)
{
  auto &buyOrders  = buy_level->getOrders();
  auto &sellOrders = sell_level->getOrders();

  for(auto buyOrder : buyOrders)
    {
      for(auto sellOrder : sellOrders)
        {
          if(buyOrder->getUserId() == sellOrder->getUserId()) { break; }
          if(buyOrder->getQuantity() <= 0)
            {
              break;
            } // No need to match if the buy order is filled

          // Calculate the quantity to trade
          int quantityTraded =
            std::min(buyOrder->getQuantity(), sellOrder->getQuantity());

          // Update quantities
          buyOrder->updateQuantity(-quantityTraded);
          sellOrder->updateQuantity(-quantityTraded);

          spdlog::info(
            "[ Matched ] {} order ID {} with {} order ID {}: {} units traded "
            "at price {} buyer {}, seller {}",
            (buyOrder->getType() == OrderType::BUY) ? "BUY" : "SELL",
            buyOrder->getId(),
            (sellOrder->getType() == OrderType::BUY) ? "BUY" : "SELL",
            sellOrder->getId(), quantityTraded, sellOrder->getPrice(),
            buyOrder->getUserId(), sellOrder->getUserId());

          // Remove filled orders
          if(sellOrder->getQuantity() == 0)
            {
              sell_level->removeOrder(
                sellOrder); // Remove from sell level if filled
            }
        }

      // If the buy order is filled, no need to check further
      if(buyOrder->getQuantity() == 0)
        {
          buy_level->removeOrder(buyOrder); // Remove from buy level if filled
        }
    }
}

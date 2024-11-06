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
      std::unique_lock lock(mtx);

      // If there are no buy orders at this price, create a new PriceLevel
      if(buy_orders.find(price) == buy_orders.end())
        {
          buy_orders[price] = std::make_unique<PriceLevel>(price);
        }
      buy_orders[price]->addOrder(std::move(new_order));
    }
  else if(type == OrderType::SELL)
    {
      std::unique_lock lock(mtx);

      // If there are no sell orders at this price, create a new PriceLevel
      if(sell_orders.find(price) == sell_orders.end())
        {
          sell_orders[price] = std::make_unique<PriceLevel>(price);
        }
      sell_orders[price]->addOrder(std::move(new_order)); // Move the order into the price level
    }
}

void OrderBook::match()
{
  // Acquire a unique lock for write operations on the order book
  std::unique_lock lock(mtx);

  // Iterate through all buy orders
  for(auto it_buy = buy_orders.begin(); it_buy != buy_orders.end();)
    {
      double bid_price = it_buy->first;               // Get the buy price
      auto  &buyOrders = it_buy->second->getOrders(); // Get the buy orders

      // Iterate through all sell orders
      for(auto it_sell = sell_orders.begin(); it_sell != sell_orders.end();)
        {
          double ask_price = it_sell->first; // Get the sell price

          // Check if we can match the buy and sell orders
          if(!canMatch(bid_price, ask_price))
            {
              ++it_sell; // Move to the next sell level if no match
              continue;
            }

          auto &sellOrders = it_sell->second->getOrders();

          for(auto buyOrderIt = buyOrders.begin(); buyOrderIt != buyOrders.end();)
            {
              auto &buyOrder = *buyOrderIt;

              for(auto sellOrderIt = sellOrders.begin(); sellOrderIt != sellOrders.end();)
                {
                  auto &sellOrder = *sellOrderIt;

                  // pointer not ready
                  if(!sellOrder || !buyOrder)
                    {
                      ++sellOrderIt;
                      continue;
                    }

                  // Prevent self-trading
                  if(buyOrder->getUserId() == sellOrder->getUserId())
                    {
                      ++sellOrderIt;
                      continue;
                    }

                  auto buyer  = buyOrder->getUserId();
                  auto seller = sellOrder->getUserId();

                  int bid_id = buyOrder->getId();
                  int ask_id = sellOrder->getId();

                  // Calculate the quantity to trade
                  int quantityTraded = std::min(buyOrder->getQuantity(), sellOrder->getQuantity());

                  // Update quantities
                  buyOrder->updateQuantity(-quantityTraded);
                  sellOrder->updateQuantity(-quantityTraded);

                  // log empty line
                  spdlog::debug("");
                  spdlog::debug("[match] sell_id : {:03}", ask_id);
                  spdlog::debug("[match] bid_id : {:03}", bid_id);
                  spdlog::debug("[match] buyer : {:03}", buyer);
                  spdlog::debug("[match] seller : {:03}", seller);
                  spdlog::debug("[match] quantity : {:03}", quantityTraded);
                  spdlog::debug("[match] price : {:03}", ask_price);

                  // Remove filled orders
                  if(sellOrder->getQuantity() == 0) { sellOrderIt = sellOrders.erase(sellOrderIt); }

                  else { ++sellOrderIt; }

                  if(buyOrder->getQuantity() == 0)
                    {
                      buyOrderIt = buyOrders.erase(buyOrderIt);
                      break; // Break out of the inner loop if the buy order is filled
                    }
                }

              if(buyOrderIt == buyOrders.end())
                {
                  break; // Break out of the buy order loop if all buy orders are matched
                }
              else { ++buyOrderIt; }
            }

          // Check if the sell level is empty after processing
          if(sellOrders.empty())
            {
              it_sell = sell_orders.erase(it_sell); // Erase and get the next iterator
            }
          else
            {
              ++it_sell; // Move to the next sell level
            }

          // Check if the buy level is empty after processing
          if(buyOrders.empty())
            {
              it_buy = buy_orders.erase(it_buy); // Erase and get the next iterator
              break; // Break out of the sell order loop since all buy orders are matched
            }
        }

      // If no matches were made for this buy level, move to the next buy level
      if(it_buy != buy_orders.end()) { ++it_buy; }
    }
}

// Helper method to check if a buy and sell order can match
bool OrderBook::canMatch(double buy_price, double sell_price) { return buy_price >= sell_price; }

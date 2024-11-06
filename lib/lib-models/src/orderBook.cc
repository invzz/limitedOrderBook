#include "orderBook.hh"
#include "spdlog/spdlog.h"

void OrderBook::addOrder(std::shared_ptr<Order> new_order)
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
      if(bids.find(price) == bids.end()) { bids[price] = std::make_shared<PriceLevel>(price); }
      bids[price]->addOrder(new_order); // No need for std::move() since we're using shared_ptr
    }
  else if(type == OrderType::SELL)
    {
      std::unique_lock lock(mtx);

      // If there are no sell orders at this price, create a new PriceLevel
      if(asks.find(price) == asks.end()) { asks[price] = std::make_shared<PriceLevel>(price); }
      asks[price]->addOrder(new_order); // No need for std::move() since we're using shared_ptr
    }
}

void OrderBook::match()
{
  // Acquire a unique lock for write operations on the order book
  std::unique_lock lock(mtx);

  // Iterate through all buy orders
  for(auto it_buy = bids.begin(); it_buy != bids.end();)
    {
      double bid_price = it_buy->first;               // Get the buy price
      auto  &buyOrders = it_buy->second->getOrders(); // Get the buy orders

      // Iterate through all sell orders
      for(auto it_sell = asks.begin(); it_sell != asks.end();)
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
              it_sell = asks.erase(it_sell); // Erase and get the next iterator
            }
          else
            {
              ++it_sell; // Move to the next sell level
            }

          // Check if the buy level is empty after processing
          if(buyOrders.empty())
            {
              it_buy = bids.erase(it_buy); // Erase and get the next iterator
              break; // Break out of the sell order loop since all buy orders are matched
            }
        }

      // If no matches were made for this buy level, move to the next buy level
      if(it_buy != bids.end()) { ++it_buy; }
    }
}

// Helper method to check if a buy and sell order can match
bool OrderBook::canMatch(double buy_price, double sell_price) { return buy_price >= sell_price; }

std::vector<std::shared_ptr<Order>> OrderBook::getBestBid()
{
  std::unique_lock lock(mtx); // Lock for thread safety

  // Check if there are any buy orders
  if(bids.empty())
    {
      spdlog::warn("No buy orders available.");
      return {}; // Return empty vector if no buy orders
    }

  // Get the highest price from the buy orders (best bid)
  double bestBidPrice = bids.rbegin()->first; // rbegin gives the iterator to the highest key

  // Return a copy of the orders at the best bid price level
  return std::move(bids.rbegin()->second->getOrders());
}

std::vector<std::shared_ptr<Order>> OrderBook::getBestAsk()
{
  std::unique_lock lock(mtx); // Lock for thread safety

  // Check if there are any sell orders
  if(asks.empty())
    {
      spdlog::warn("No sell orders available.");
      return {}; // Return empty vector if no sell orders
    }

  // Get the lowest price from the sell orders (best ask)
  auto bestAsk    = asks.begin(); // begin gives the iterator to the lowest key
  auto PriceLevel = bestAsk->second;
  // Return a copy of the orders at the best ask price level
  return std::move(PriceLevel->getOrders());
}

nlohmann::json OrderBook::toJson()
{
  std::shared_lock lock(mtx);
  nlohmann::json   json;
  {
    // Serialize buy orders
    nlohmann::json buyOrdersJson = nlohmann::json::array();
    for(const auto &[price, level] : bids)
      {
        if(level) buyOrdersJson.push_back(level->toJson());
      }
    json["buy_orders"] = buyOrdersJson;

    // Serialize sell orders
    nlohmann::json sellOrdersJson = nlohmann::json::array();
    for(const auto &[price, level] : asks)
      {
        if(level) sellOrdersJson.push_back(level->toJson());
      }
    json["sell_orders"] = sellOrdersJson;
  }
  return json;
}

std::unique_ptr<OrderBook> OrderBook::fromJson(const nlohmann::json &orderBookData)
{
  auto orderBook = std::make_unique<OrderBook>();

  // Deserialize buy orders
  for(const auto &buyLevelData : orderBookData["buy_orders"])
    {
      auto buyLevel                              = PriceLevel::fromJson(buyLevelData);
      orderBook->getBids()[buyLevel->getPrice()] = std::move(buyLevel);
    }

  // Deserialize sell orders
  for(const auto &sellLevelData : orderBookData["sell_orders"])
    {
      auto sellLevel                              = PriceLevel::fromJson(sellLevelData);
      orderBook->getAsks()[sellLevel->getPrice()] = std::move(sellLevel);
    }

  return orderBook;
}

#include "sellerBot.hh"
#include "marketSimulator.hh"
#include <spdlog/spdlog.h>

void SellerBot::run()
{
  // Get the order book from the server
  auto &orderBook = server->getOrderBook();
  auto &buyOrders = orderBook.getBuyOrders();

  // Analyze current buy orders to adjust selling strategy
  if(!buyOrders.empty())
    {
      double totalPrice    = 0.0;
      int    totalQuantity = 0;

      // Iterate through buy orders to calculate the total price and quantity
      for(const auto &order : buyOrders)
        {
          double      price      = order.first;  // The key of the map is the price
          const auto &priceLevel = order.second; // The value is a unique_ptr to PriceLevel

          // Iterate through all orders at this price level to calculate total quantity
          for(const auto &orderPtr : priceLevel->getOrders())
            {
              totalQuantity += orderPtr->getQuantity(); // Assuming Order has a getQuantity() method
              totalPrice += price * orderPtr->getQuantity(); // price * quantity
            }
        }

      // Avoid division by zero
      if(totalQuantity > 0)
        {
          double averageBuyPrice = totalPrice / totalQuantity;

          // Set min price based on the average buy price
          min_price = std::max(min_price, averageBuyPrice * 0.95); // 5% below the average
          max_price = std::min(max_price, averageBuyPrice * 1.05); // 5% above the average
        }
    }

  // Ensure valid price range
  if(min_price >= max_price)
    {
      return; // Skip this turn if the price range is invalid
    }

  // Generate a random quantity to sell, influenced by market conditions
  std::uniform_int_distribution<int> quantity_dist(1, max_quantity);
  int                                quantity = quantity_dist(rng);

  // Generate an order to sell at a price between min and max price
  std::uniform_real_distribution<double> price_dist(min_price, max_price);
  double                                 sellingPrice = price_dist(rng);

  auto sellOrder = std::make_unique<Order>(OrderType::SELL, sellingPrice, quantity, userId);

  // Add the order to the order book
  server->addOrder(std::move(sellOrder));
}
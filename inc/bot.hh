#include "orderBook.hh"
#include "order.hh"
#include <spdlog/spdlog.h>
#include <random>
#include <thread>
#include <chrono>

class Bot
{
  public:
  Bot(OrderBook &orderBook, int id, int orders_per_second, int duration_seconds,
      int userId)
      : orderBook(orderBook), goodId(id), orders_per_second(orders_per_second),
        duration_seconds(duration_seconds), userId(userId)
  {}

  void operator()()
  {
    std::mt19937                           rng(std::random_device{}());
    std::uniform_int_distribution<int>     quantity_dist(1, 100);
    std::uniform_real_distribution<double> price_dist(90.0, 110.0);
    std::uniform_int_distribution<int> type_dist(0, 1); // 0 for BUY, 1 for SELL

    auto start_time = std::chrono::high_resolution_clock::now();

    for(int second = 0; second < duration_seconds; ++second)
      {
        for(int i = 0; i < orders_per_second; ++i)
          {
            int good_id =
              goodId; // Each bot can be associated with a different good
            OrderType type =
              (type_dist(rng) == 0) ? OrderType::BUY : OrderType::SELL;
            double price    = price_dist(rng);
            int    quantity = quantity_dist(rng);

           auto new_order =
              std::make_unique<Order>(good_id, type, price, quantity, userId);
            orderBook.addOrder(new_order.release());
          }

        spdlog::warn("Bot {} placed {} orders in second {}", goodId,
                     orders_per_second, second + 1);
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end_time - start_time;

    spdlog::info("Bot {} finished placing orders. Total time taken: {}", goodId,
                 elapsed_seconds.count());
  }

  private:
  OrderBook &orderBook;
  int        goodId;
  int        orders_per_second;
  int        duration_seconds;
  int        userId;
};
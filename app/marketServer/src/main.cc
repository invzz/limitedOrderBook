#include "marketServer.hh"
#include <spdlog/spdlog.h>

int main()
{
  spdlog::set_level(spdlog::level::debug);
  MarketServer server;
  server.start();
  while(true) {}
  server.stop();
  return 0;
}
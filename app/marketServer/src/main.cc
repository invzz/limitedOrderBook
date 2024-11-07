#include "marketServer.hh"
#include <spdlog/spdlog.h>

int main()
{
  spdlog::set_level(spdlog::level::info);
  MarketServer server;
  server.start();
  while(true) { 
    
  }
  server.stop();
  return 0;
}
#include <commandFactory.hh>
#include "marketServer.hh"
#include "MultiMarketServer.hh"
namespace market
{
    // Explicit instantiation of the template for MarketServer
    template class CommandFactory<MarketServer>;
    template class CommandFactory<MultiMarketServer>;
} // namespace market
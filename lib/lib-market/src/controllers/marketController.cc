#include "marketController.hh"
#include "marketServer.hh"

namespace market
{
    // Explicit instantiation of the template for MarketServer
    template class MarketController<MarketServer>;
} // namespace market
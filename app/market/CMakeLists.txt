project(MarketApp)

# Main application source
set(SOURCES market.cc)

# Create the executable
add_executable(MarketApp ${SOURCES})

# Link against BotLib and OrderBookLib; include directories are automatically propagated
target_link_libraries(MarketApp PRIVATE lib-models lib-market-simulator spdlog::spdlog)




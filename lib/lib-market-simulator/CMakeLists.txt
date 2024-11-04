project(lib-market-simulator)

# Collect source and header files
file(GLOB LOB_HEADERS inc/*.hh)
file(GLOB LOB_SOURCES src/*.cc)

# Create a static library
add_library(${PROJECT_NAME} STATIC ${LOB_SOURCES})

# Set include directories to be exported
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Link spdlog to OrderBookLib
target_link_libraries(${PROJECT_NAME} PUBLIC lib-models spdlog::spdlog )
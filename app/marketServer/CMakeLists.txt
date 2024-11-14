project(market-server)


# Main application source
set(SOURCES src/main.cc src/marketServer.cc)


# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE cxxopts::cxxopts cppzmq-static lib-models spdlog::spdlog_header_only nlohmann_json::nlohmann_json )

# Set include directories to be exported
target_include_directories(${PROJECT_NAME} PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/inc 
${CMAKE_CURRENT_SOURCE_DIR}/inc/commands 
${CMAKE_CURRENT_SOURCE_DIR}/inc/controller
)

project(market-server)





# Main application source
set(SOURCES marketServer.cc)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE cppzmq-static  lib-models lib-market-simulator spdlog::spdlog nlohmann_json::nlohmann_json )

# target_include_directories(${PROJECT_NAME} PRIVATE ${ZeroMQ_INCLUDE_DIRS})


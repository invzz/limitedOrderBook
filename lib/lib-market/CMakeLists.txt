project(lib-market)

# Collect source and header files
file(GLOB_RECURSE ORDERBOOKLIB_HEADERS *.hh)
file(GLOB_RECURSE ORDERBOOKLIB_SOURCES *.cc)

# Create a static library
add_library(${PROJECT_NAME} STATIC ${ORDERBOOKLIB_SOURCES} )

# Set include directories to be exported
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/commands 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/commands/abstract
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/commands/factory
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/controllers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/repositories
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/services
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/mediators
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/models
)

target_link_libraries(${PROJECT_NAME} PUBLIC cppzmq-static spdlog::spdlog_header_only nlohmann_json::nlohmann_json)


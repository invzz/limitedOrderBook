cmake_minimum_required(VERSION 3.10)
project(lib-market)

set(CMAKE_CXX_STANDARD 17)

# Collect source and header files
file(GLOB_RECURSE ORDERBOOKLIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hh)
file(GLOB_RECURSE ORDERBOOKLIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

# Create a static library
add_library(${PROJECT_NAME}-static STATIC ${ORDERBOOKLIB_SOURCES})

# Set include directories to be exported
target_include_directories(${PROJECT_NAME}-static PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/commands>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/commands/abstract>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/commands/factory>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/controllers>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/repositories>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/services>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/mediators>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/models>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/routers>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/formatters>
)

target_link_libraries(${PROJECT_NAME}-static PUBLIC
    spdlog::spdlog_header_only
    cppzmq-static
)


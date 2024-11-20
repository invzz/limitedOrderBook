project(lib-models)

# Collect source and header files
file(GLOB ORDERBOOKLIB_HEADERS inc/*hh)
file(GLOB ORDERBOOKLIB_SOURCES src/*.cc)

# Create a static library
add_library(${PROJECT_NAME} INTERFACE ${ORDERBOOKLIB_SOURCES} )

# Set include directories to be exported
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Link spdlog to OrderBookLib
target_link_libraries(${PROJECT_NAME} INTERFACE spdlog::spdlog nlohmann_json::nlohmann_json)

# Add tests
# enable_testing()
# file(GLOB TEST_SOURCES "test/*.cc")


# foreach(TEST_SOURCE ${TEST_SOURCES})
#     # Get the filename without the path or extension to name the executable
#     get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

#     # Add an executable for each test source file
#     add_executable(${TEST_NAME} ${TEST_SOURCE})

#     # Link each test executable to the OrderBook library and GTest
#     target_link_libraries(${TEST_NAME} PRIVATE lib-models GTest::gtest )

#     # Link spdlog to each test executable if needed
#     target_link_libraries(${TEST_NAME} PRIVATE spdlog::spdlog)

#     # Register each test with CTest
#     add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
# endforeach()
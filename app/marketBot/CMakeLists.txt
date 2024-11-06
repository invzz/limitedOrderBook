project(marketbot)


find_package(cppzmq REQUIRED)

# Main application source
set(SOURCES marketbot.cc )

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE cppzmq cppzmq-static lib-models spdlog::spdlog)
# Set include directories to be exported
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)



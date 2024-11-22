cmake_minimum_required(VERSION 3.10)

# Define the project name and C++ standard globally
project(server-side-app VERSION 1.0)

# Set the sources for the executable
set(SOURCES src/main.cc)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the lib-market package to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE lib-market-static spdlog::spdlog_header_only)
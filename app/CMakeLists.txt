cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Define the project name and C++ standard globally
project(MarketSimulator VERSION 1.0)

set(APPS marketServer marketBot  )

foreach(APP ${APPS})
    add_subdirectory(${APP})
endforeach()
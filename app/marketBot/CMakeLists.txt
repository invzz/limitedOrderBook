project(marketbot)
set(SOURCES src/main.cc )
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE cppzmq-static lib-market-static spdlog::spdlog_header_only)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

file(GLOB_RECURSE files *.cc *.hh)
foreach(line IN LISTS files)
    string(APPEND multiline "[${PROJECT_NAME}] :: ${line}\n")
endforeach() 
message("${multiline}")



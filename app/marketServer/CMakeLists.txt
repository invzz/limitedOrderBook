project(server-side-app)
set(SOURCES src/main.cc )
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE lib-market  )


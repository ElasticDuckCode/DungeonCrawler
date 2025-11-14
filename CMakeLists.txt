cmake_minimum_required(VERSION 3.31.6)
project(crawler VERSION 0.0.1)

add_executable(${PROJECT_NAME} src/main.cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
    PUBLIC ${SDL2_LIBRARIES}
)


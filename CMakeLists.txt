cmake_minimum_required(VERSION 3.31.6)
project(crawler VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE PROJECT_SOURCES
        "${CMAKE_SOURCE_DIR}/src/*.c"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        )

add_subdirectory(extern/SDL EXCLUDE_FROM_ALL)
add_subdirectory(extern/Eigen EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

target_link_libraries(${PROJECT_NAME}
        PRIVATE SDL3::SDL3
        PRIVATE Eigen3::Eigen
)


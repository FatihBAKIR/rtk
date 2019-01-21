cmake_minimum_required(VERSION 3.5)
project(renderer)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

add_subdirectory(rtk)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(renderer tests/main.cpp)

find_package(OpenCV REQUIRED)

target_include_directories(renderer PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(renderer PUBLIC rtk ${OpenCV_LIBS})
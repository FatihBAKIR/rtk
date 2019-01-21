cmake_minimum_required(VERSION 3.5)
project(renderer)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

add_subdirectory(rtk)
add_subdirectory(3rd_party/imgui)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_subdirectory(glad_debug)
else()
    add_subdirectory(glad)
endif()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(renderer tests/main.cpp tests/imgui_glfw.hpp tests/imgui_glfw.cpp)

target_link_libraries(renderer PUBLIC rtk imgui)
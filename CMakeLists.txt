cmake_minimum_required(VERSION 3.5)
project(renderer)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

add_subdirectory(rtk)
add_subdirectory(3rd_party/imgui)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_subdirectory(glad_debug)
else()
    add_subdirectory(glad)
endif()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(renderer_lib tests/imgui_glfw.hpp tests/imgui_glfw.cpp tests/material.hpp tests/lighting.hpp tests/renderable.hpp tests/scene.hpp tests/lighting.cpp tests/cam_controller.hpp tests/shader_man.hpp tests/shader_man.cpp tests/cam_controller.cpp tests/renderer.cpp)
target_include_directories(renderer_lib PUBLIC tests)
target_link_libraries(renderer_lib PUBLIC rtk imgui)

add_executable(renderer tests/main.cpp)
target_link_libraries(renderer renderer_lib)

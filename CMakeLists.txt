cmake_minimum_required(VERSION 3.5)
project(renderer)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

set(CMAKE_CXX_STANDARD 14)

set(RTK_ROOT "${CMAKE_SOURCE_DIR}/rtk")

set(SOURCE_FILES tests/main.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(renderer ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_include_directories(renderer PUBLIC ./glad_debug/include)
else()
    target_include_directories(renderer PUBLIC ./glad/include)
endif()

target_include_directories(renderer PUBLIC ./3rd_party/gsl/include)

find_package(RTK REQUIRED)
target_link_libraries(renderer ${RTK_LIBRARIES})
target_include_directories(renderer PUBLIC ${RTK_INCLUDE_DIRS})
target_compile_definitions(renderer PUBLIC ${RTK_COMPILE_FLAGS})

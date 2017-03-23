cmake_minimum_required(VERSION 3.6)
project(renderer)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

set(GLAD_FILES glad/src/glad.c glad/include/glad/glad.h glad/include/KHR/khrplatform.h)

set(SOURCE_FILES rtk/main.cpp rtk/window.cpp rtk/window.hpp rtk/rtk_init.cpp rtk/rtk_init.hpp rtk/display.cpp rtk/display.hpp rtk/graphics/units.hpp rtk/graphics/size.cpp rtk/graphics/size.hpp rtk/rtk_fwd.hpp)
add_executable(renderer ${SOURCE_FILES} ${GLAD_FILES})

target_include_directories(renderer PUBLIC .)
target_include_directories(renderer PUBLIC ./glad/include)

target_link_libraries(renderer ${OPENGL_LIBRARIES})
target_link_libraries(renderer ${GLFW_LIBRARIES})
target_link_libraries(renderer ${Boost_LIBRARIES})
target_link_libraries(renderer m dl)

target_include_directories(renderer PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(renderer PUBLIC ${GLFW_INCLUDE_DIR})
target_include_directories(renderer PUBLIC ${Boost_INCLUDE_DIRS})

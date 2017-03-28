cmake_minimum_required(VERSION 3.6)
project(renderer)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(assimp REQUIRED)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(GLAD_FILES glad_debug/src/glad.c glad_debug/include/glad/glad.h glad_debug/include/KHR/khrplatform.h)
else()
    set(GLAD_FILES glad/src/glad.c glad/include/glad/glad.h glad/include/KHR/khrplatform.h)
endif()

set(SOURCE_FILES rtk/main.cpp rtk/window.cpp rtk/window.hpp rtk/rtk_init.cpp rtk/rtk_init.hpp rtk/display.cpp rtk/display.hpp rtk/graphics/units.hpp rtk/graphics/size.cpp rtk/graphics/size.hpp rtk/rtk_fwd.hpp rtk/geometry/mesh.cpp rtk/geometry/mesh.hpp rtk/gl/shader.cpp rtk/gl/shader.hpp rtk/gl/program.cpp rtk/gl/program.hpp rtk/gl/vertex_buffer.cpp rtk/gl/vertex_buffer.hpp rtk/asset/mesh_import.cpp rtk/asset/mesh_import.hpp rtk/gl/mesh.cpp rtk/gl/mesh.hpp)
add_executable(renderer ${SOURCE_FILES} ${GLAD_FILES})

target_compile_definitions(renderer PRIVATE GLFW_INCLUDE_NONE=1)

target_include_directories(renderer PUBLIC .)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_include_directories(renderer PUBLIC ./glad_debug/include)
else()
    target_include_directories(renderer PUBLIC ./glad/include)
endif()

target_include_directories(renderer PUBLIC ./3rd_party/gsl/include)
target_include_directories(renderer PUBLIC ./3rd_party/glm/)

target_link_libraries(renderer ${OPENGL_LIBRARIES})
target_link_libraries(renderer ${GLFW_LIBRARIES})
target_link_libraries(renderer ${Boost_LIBRARIES})
target_link_libraries(renderer ${assimp_LIBRARIES})
target_link_libraries(renderer m dl)

target_include_directories(renderer PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(renderer PUBLIC ${GLFW_INCLUDE_DIR})
target_include_directories(renderer PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(renderer PUBLIC ${assimp_INCLUDE_DIRS})

cmake_minimum_required(VERSION 3.5)
project(rtk)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake_modules/")

set(CMAKE_CXX_STANDARD 14)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(assimp REQUIRED)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(GLAD_FILES ../glad_debug/src/glad.c)
else()
    set(GLAD_FILES ../glad/src/glad.c)
endif()

set(SOURCE_FILES window.cpp window.hpp rtk_init.cpp rtk_init.hpp display.cpp display.hpp graphics/units.hpp
        graphics/size.hpp rtk_fwd.hpp geometry/mesh.cpp geometry/mesh.hpp gl/shader.hpp gl/program.cpp gl/program.hpp
        gl/vertex_buffer.hpp asset/mesh_import.cpp asset/mesh_import.hpp gl/mesh.cpp gl/mesh.hpp
        geometry/path.cpp geometry/path.hpp gl/path.cpp gl/path.hpp mesh_ops.hpp mesh_ops.cpp)

add_library(rtk STATIC ${SOURCE_FILES} ${GLAD_FILES})

target_compile_definitions(rtk PRIVATE GLFW_INCLUDE_NONE=1)

target_include_directories(rtk PUBLIC ..)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_include_directories(rtk PUBLIC ../glad_debug/include)
    target_compile_definitions(rtk PRIVATE RTK_DEBUG)
else()
    target_include_directories(rtk PUBLIC ../glad/include)
endif()

target_compile_definitions(rtk PRIVATE GLM_ENABLE_EXPERIMENTAL)

target_include_directories(rtk PUBLIC ../3rd_party/gsl/include)
target_include_directories(rtk PUBLIC ../3rd_party/glm/)

target_link_libraries(rtk ${OPENGL_LIBRARIES})
target_link_libraries(rtk ${GLFW_LIBRARIES})
target_link_libraries(rtk ${Boost_LIBRARIES})
target_link_libraries(rtk ${assimp_LIBRARIES})
target_link_libraries(rtk m dl)

target_include_directories(rtk PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(rtk PUBLIC ${GLFW_INCLUDE_DIR})
target_include_directories(rtk PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(rtk PUBLIC ${assimp_INCLUDE_DIRS})

cmake_minimum_required(VERSION 3.6)
project(renderer)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(assimp REQUIRED)

link_directories(rtk/bui)
set(SOURCE_FILES tests/main.cpp glad_debug/src/glad.c)
add_executable(renderer ${SOURCE_FILES})

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
target_link_libraries(renderer m dl rtk)

target_include_directories(renderer PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(renderer PUBLIC ${GLFW_INCLUDE_DIR})
target_include_directories(renderer PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(renderer PUBLIC ${assimp_INCLUDE_DIRS})

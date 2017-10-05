cmake_minimum_required(VERSION 3.5)
project(rtk)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake_modules/")

option(RTK_INCLUDE_CAMERA "Build camera" OFF)
option(RTK_GLES2 "GLES2 Friendly build" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(assimp REQUIRED)
find_package(GLM REQUIRED)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(GLAD_FILES ../glad_debug/src/glad.c)
else()
    set(GLAD_FILES ../glad/src/glad.c)
endif()

set(HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/rtk")

set(HEADER_FILES
        ${HEADER_PATH}/window.hpp
        ${HEADER_PATH}/rtk_init.hpp
        ${HEADER_PATH}/display.hpp
        ${HEADER_PATH}/graphics/units.hpp
        ${HEADER_PATH}/graphics/size.hpp
        ${HEADER_PATH}/graphics/rect.hpp
        ${HEADER_PATH}/rtk_fwd.hpp
        ${HEADER_PATH}/gl/program.hpp
        ${HEADER_PATH}/geometry/mesh.hpp
        ${HEADER_PATH}/gl/shader.hpp
        ${HEADER_PATH}/asset/mesh_import.hpp
        ${HEADER_PATH}/asset/rtk_assimp.hpp
        ${HEADER_PATH}/gl/mesh.hpp
        ${HEADER_PATH}/geometry/path.hpp
        ${HEADER_PATH}/gl/path.hpp
        ${HEADER_PATH}/mesh_ops.hpp
        ${HEADER_PATH}/physics/aabb.hpp
        ${HEADER_PATH}/camera.hpp
        ${HEADER_PATH}/utility.hpp
        ${HEADER_PATH}/texture/tex2d.hpp
        ${HEADER_PATH}/framebuffer.hpp
        )

set(DESKTOP_FILES src/display.cpp src/window.cpp src/rtk_init.cpp src/geometry/path.cpp src/gl/path.cpp)

set(SOURCE_FILES
        src/geometry/mesh.cpp src/gl/program.cpp src/asset/mesh_import.cpp src/gl/mesh.cpp
        src/mesh_ops.cpp src/utility.cpp
        src/texture/tex2d.cpp src/framebuffer.cpp)

if (RTK_INCLUDE_CAMERA)
    set(SOURCE_FILES ${SOURCE_FILES} src/camera.cpp)
    set(HEADER_FILES ${HEADER_FILES} ${HEADER_PATH}/camera.hpp)
endif()

if (RTK_CONTEXT_CREATE)
    set(SOURCE_FILES ${SOURCE_FILES} ${DESKTOP_FILES})
endif()

add_library(glad SHARED ${GLAD_FILES})

add_library(rtk SHARED ${SOURCE_FILES} ${HEADER_FILES})
if (RTK_GLES2)
    target_compile_definitions(rtk PUBLIC RTK_GLES2)
else()
    target_link_libraries(rtk PUBLIC glad)
endif()

target_compile_definitions(rtk PUBLIC GLFW_INCLUDE_NONE=1)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(rtk PUBLIC RTK_DEBUG)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../glad_debug/include/" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/include/")
else()
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../glad/include/" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/include/")
endif()

target_compile_definitions(rtk PUBLIC GLM_ENABLE_EXPERIMENTAL)
target_compile_definitions(rtk PUBLIC RTK_NAMESPACE=rtk)

target_include_directories(rtk PUBLIC ${GLM_INCLUDE_DIRS})

target_link_libraries(rtk PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(rtk PUBLIC ${GLFW_LIBRARIES})
target_link_libraries(rtk PUBLIC ${Boost_LIBRARIES})
target_link_libraries(rtk PUBLIC assimp)
target_link_libraries(rtk PUBLIC SOIL)

target_include_directories(rtk PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(rtk PUBLIC ${GLFW_INCLUDE_DIR})
target_include_directories(rtk PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(rtk PUBLIC ${assimp_INCLUDE_DIRS})

set_target_properties(rtk PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(rtk PROPERTIES C_VISIBILITY_PRESET hidden)

set(INCLUDE_DEST "include")
set(LIB_DEST "lib/rtk")

target_include_directories(rtk PUBLIC
        $<BUILD_INTERFACE:${HEADER_PATH}/../>
        $<INSTALL_INTERFACE:${INCLUDE_DEST}>)

target_include_directories(glad PUBLIC
        $<BUILD_INTERFACE:${HEADER_PATH}/../>
        $<INSTALL_INTERFACE:${INCLUDE_DEST}>)

install(TARGETS glad DESTINATION "${LIB_DEST}")
install(TARGETS rtk DESTINATION "${LIB_DEST}")

install(FILES rtk-config.cmake DESTINATION ${LIB_DEST})
install(FILES glad-config.cmake DESTINATION ${LIB_DEST})

install(TARGETS rtk glad EXPORT rtk DESTINATION "${LIB_DEST}")
install(EXPORT rtk DESTINATION "${LIB_DEST}")

install(DIRECTORY ${HEADER_PATH} DESTINATION "${INCLUDE_DEST}")
install(DIRECTORY ${HEADER_PATH}/../glad DESTINATION "${INCLUDE_DEST}")
install(DIRECTORY ${HEADER_PATH}/../KHR DESTINATION "${INCLUDE_DEST}")

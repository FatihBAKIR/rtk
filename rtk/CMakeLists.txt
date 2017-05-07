cmake_minimum_required(VERSION 3.5)
project(rtk)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake_modules/")

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
        ${HEADER_PATH}/gl/vertex_buffer.hpp
        ${HEADER_PATH}/geometry/mesh.hpp
        ${HEADER_PATH}/gl/shader.hpp
        ${HEADER_PATH}/asset/mesh_import.hpp
        ${HEADER_PATH}/gl/mesh.hpp
        ${HEADER_PATH}/geometry/path.hpp
        ${HEADER_PATH}/gl/path.hpp
        ${HEADER_PATH}/mesh_ops.hpp
        ${HEADER_PATH}/physics/aabb.hpp
        ${HEADER_PATH}/camera.hpp
        ${HEADER_PATH}/utility.hpp
        )

set(SOURCE_FILES window.cpp rtk_init.cpp display.cpp
       geometry/mesh.cpp gl/program.cpp asset/mesh_import.cpp gl/mesh.cpp geometry/path.cpp gl/path.cpp mesh_ops.cpp camera.cpp utility.cpp)


add_library(rtk SHARED ${SOURCE_FILES} ${HEADER_FILES} ${GLAD_FILES})

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

target_link_libraries(rtk ${OPENGL_LIBRARIES})
target_link_libraries(rtk ${GLFW_LIBRARIES})
target_link_libraries(rtk ${Boost_LIBRARIES})
target_link_libraries(rtk ${assimp_LIBRARIES})
target_link_libraries(rtk m dl)

target_include_directories(rtk PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(rtk PUBLIC ${GLFW_INCLUDE_DIR})
target_include_directories(rtk PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(rtk PUBLIC ${assimp_INCLUDE_DIRS})

set(INCLUDE_DEST "include")
set(LIB_DEST "lib/rtk")

target_include_directories(rtk PUBLIC
        $<BUILD_INTERFACE:${HEADER_PATH}/../>
        $<INSTALL_INTERFACE:${INCLUDE_DEST}>)

install(TARGETS rtk DESTINATION "${LIB_DEST}")
install(FILES rtk-config.cmake DESTINATION ${LIB_DEST})
install(DIRECTORY ${HEADER_PATH} DESTINATION "${INCLUDE_DEST}")
install(DIRECTORY ${HEADER_PATH}/../glad DESTINATION "${INCLUDE_DEST}")
install(DIRECTORY ${HEADER_PATH}/../KHR DESTINATION "${INCLUDE_DEST}")

install(TARGETS rtk EXPORT rtk DESTINATION "${LIB_DEST}")
install(EXPORT rtk DESTINATION "${LIB_DEST}")

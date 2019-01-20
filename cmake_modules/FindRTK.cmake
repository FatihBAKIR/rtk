find_path(RTK_INCLUDE_DIR
        NAMES rtk/rtk_fwd.hpp
        PATHS /usr/local/include /usr/include "${RTK_ROOT}/include")

find_library(RTK_LIBRARIES
        NAMES rtk
        PATHS /usr/local/lib /usr/lib "${RTK_ROOT}/build")

find_path(GLAD_INCLUDE_DIR
        NAMES glad/glad.h KHR/khrplatform.h
        PATHS /usr/local/include /usr/include "${RTK_ROOT}/include")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RTK DEFAULT_MSG RTK_INCLUDE_DIR)
if (RTK_FOUND)
    find_package(GLFW REQUIRED QUIET)
    find_package(GLM REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(assimp REQUIRED QUIET)
    find_package(Boost REQUIRED COMPONENTS system QUIET)

    if (NOT RTK_FIND_QUIETLY)
        message(STATUS "Found RTK")
    endif()

    set(RTK_INCLUDE_DIRS "")
    list(APPEND RTK_INCLUDE_DIRS "${RTK_INCLUDE_DIR}")
    list(APPEND RTK_INCLUDE_DIRS "${GLFW_INCLUDE_DIRS}")
    list(APPEND RTK_INCLUDE_DIRS "${OPENGL_INCLUDE_DIR}")
    list(APPEND RTK_INCLUDE_DIRS "${assimp_INCLUDE_DIRS}")
    list(APPEND RTK_INCLUDE_DIRS "${BOOST_INCLUDE_DIRS}")
    list(APPEND RTK_INCLUDE_DIRS "${GLM_INCLUDE_DIRS}")
    list(APPEND RTK_INCLUDE_DIRS "${GLAD_INCLUDE_DIRS}")

    list(APPEND RTK_LIBRARIES "${OPENGL_LIBRARIES}")
    list(APPEND RTK_LIBRARIES "${GLFW_LIBRARIES}")
    list(APPEND RTK_LIBRARIES "${assimp_LIBRARIES}")
    list(APPEND RTK_LIBRARIES "${BOOST_LIBRARIES}")
    list(APPEND RTK_LIBRARIES "m")
    list(APPEND RTK_LIBRARIES "dl")

    set(RTK_COMPILE_FLAGS "GLFW_INCLUDE_NONE=1 -DRTK_NAMESPACE=rtk")
endif()

mark_as_advanced(RTK_INCLUDE_DIR)

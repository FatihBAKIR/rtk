//
// Created by fatih on 19.03.2017.
//

#include "glfw.hpp"
#include <GLFW/glfw3.h>

namespace rtk
{
    int glfw_init::init_count = 0;

    glfw_init::glfw_init() {
        if (init_count == 0)
        {
            ::glfwInit();
        }
        ++init_count;
    }

    glfw_init::~glfw_init() {
        if (init_count == 1)
        {
            ::glfwTerminate();
        }
        --init_count;
    }
}
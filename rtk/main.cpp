#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glfw.hpp"
#include <rtk/window.hpp>
#include <rtk/graphics/units.hpp>


int main() {
    rtk::glfw_init init;

    using namespace rtk::literals;

    rtk::window w({800_px, 600_px});

    return 0;
}
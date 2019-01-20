//
// Created by fatih on 19.03.2017.
//

#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <rtk/graphics/size.hpp>
#include <rtk/rtk_fwd.hpp>

namespace RTK_NAMESPACE
{
    class RTK_PUBLIC display {
        GLFWmonitor* opaque;

        display(GLFWmonitor* mon);
        friend std::vector<display> get_displays();
        friend display get_primary_display();
        friend class window;
    public:
        const char* get_name() const;

        physical_size get_physical_size() const;
        resolution get_resolution() const;
    };

    std::vector<display> get_displays() RTK_PUBLIC;
    display get_primary_display() RTK_PUBLIC;
}


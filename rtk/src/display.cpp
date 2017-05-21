//
// Created by fatih on 19.03.2017.
//

#include <rtk/display.hpp>
#include <GLFW/glfw3.h>

namespace RTK_NAMESPACE
{
    std::vector<display> get_displays() {
        int mon_c;
        auto monitors = glfwGetMonitors(&mon_c);

        std::vector<display> disps;
        for (int i = 0; i < mon_c; ++i)
        {
            disps.push_back(monitors[i]);
        }
        return disps;
    }

    display get_primary_display() {
        return { glfwGetPrimaryMonitor() };
    }

    display::display(GLFWmonitor* mon) :
        opaque(mon)
    {
    }

    const char *display::get_name() const {
        return glfwGetMonitorName(opaque);
    }

    physical_size display::get_physical_size() const {
        int w, h;
        glfwGetMonitorPhysicalSize(opaque, &w, &h);
        return physical_size(millimeters(w), millimeters(h));
    }

    resolution display::get_resolution() const {
        auto mode = glfwGetVideoMode(opaque);

        return resolution(pixels(mode->width), pixels(mode->height));
    }
}
//
// Created by fatih on 19.03.2017.
//

#include "display.hpp"
#include <GLFW/glfw3.h>


std::vector<rtk::display> rtk::get_displays() {
    int mon_c;
    auto monitors = glfwGetMonitors(&mon_c);

    std::vector<rtk::display> disps;
    for (int i = 0; i < mon_c; ++i)
    {
        disps.push_back(monitors[i]);
    }
    return disps;
}

rtk::display rtk::get_primary_display() {
    return { glfwGetPrimaryMonitor() };
}

rtk::display::display(GLFWmonitor* mon) :
    opaque(mon)
{
}

const char *rtk::display::get_name() const {
    return glfwGetMonitorName(opaque);
}

rtk::physical_size rtk::display::get_physical_size() const {
    int w, h;
    glfwGetMonitorPhysicalSize(opaque, &w, &h);
    return rtk::physical_size(rtk::millimeters(w), rtk::millimeters(h));
}

rtk::resolution rtk::display::get_resolution() const {
    auto mode = glfwGetVideoMode(opaque);

    return rtk::resolution(rtk::pixels(mode->width), rtk::pixels(mode->height));
}


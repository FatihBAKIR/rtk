//
// Created by fatih on 19.03.2017.
//

#pragma once

#include <rtk/graphics/size.hpp>
#include <rtk/rtk_fwd.hpp>
#include <GLFW/glfw3.h>
#include <rtk/display.hpp>

namespace rtk
{

    class window
    {
        GLFWwindow* wnd;
    public:
        window(resolution res, const std::string& title = "rtk window");
        ~window();
    };
}


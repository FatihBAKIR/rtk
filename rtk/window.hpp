//
// Created by fatih on 19.03.2017.
//

#pragma once

#include <rtk/graphics/size.hpp>
#include <rtk/rtk_fwd.hpp>
#include <GLFW/glfw3.h>
#include <rtk/display.hpp>
#include <string>

namespace rtk
{

    class window
    {
        GLFWwindow* wnd;
    public:
        window(resolution res, const std::string& title = std::string("rtk window"));

        void begin_draw();
        void end_draw();

        ~window();
    };
}


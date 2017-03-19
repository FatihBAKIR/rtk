//
// Created by fatih on 19.03.2017.
//

#include "window.hpp"
#include <rtk/display.hpp>
#include <cassert>

rtk::window::window(rtk::resolution res, const std::string &title) {
    wnd = glfwCreateWindow(res.width, res.height, title.c_str(), nullptr, nullptr);

    assert(wnd);
}

rtk::window::~window() {
    glfwDestroyWindow(wnd);
}

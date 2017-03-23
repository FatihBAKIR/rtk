//
// Created by fatih on 19.03.2017.
//

#include "window.hpp"
#include <rtk/display.hpp>
#include <cassert>
#include <glad/glad.h>

namespace {
    void glad_post_handler(const char *name, void *funcptr, int len_args, ...)
    {
        GLenum error_code;
        error_code = glad_glGetError();

        if (error_code != GL_NO_ERROR)
        {
            throw std::runtime_error("something broke (" + std::string(name) + ")");
        }
    }
}

rtk::window::window(rtk::resolution res, const std::string &title) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    wnd = glfwCreateWindow(res.width, res.height, title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(wnd);
    static auto init = []{
        if (!gladLoadGL())
        {
            throw std::runtime_error("could not initialize glad");
        }

        glad_set_post_callback(glad_post_handler);
        return 1;
    }();

    assert(wnd);

    int width, height;
    glfwGetFramebufferSize(wnd, &width, &height);

    glViewport(0, 0, width, height);
}

void rtk::window::begin_draw() {
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void rtk::window::end_draw() {
    glfwSwapBuffers(wnd);
}

rtk::window::~window() {
    glfwDestroyWindow(wnd);
}

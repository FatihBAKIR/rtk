//
// Created by fatih on 19.03.2017.
//

#include "window.hpp"
#include <rtk/display.hpp>
#include <cassert>
#include <glad/glad.h>
#include <map>

namespace {
#if defined(RTK_DEBUG)
    void glad_post_handler(const char *name, void *funcptr, int len_args, ...)
    {
        GLenum error_code;
        error_code = glad_glGetError();

        if (error_code != GL_NO_ERROR)
        {
            throw std::runtime_error("something broke (" + std::string(name) + ")");
        }
    }
#endif
}

namespace rtk {
    window::window(resolution res, const std::string& title)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        wnd = glfwCreateWindow(res.width, res.height, title.c_str(), nullptr, nullptr);

        glfwMakeContextCurrent(wnd);
        static auto init = [] {
            if (!gladLoadGL()) {
                throw std::runtime_error("could not initialize glad");
            }
#if defined(RTK_DEBUG)
            glad_set_post_callback(glad_post_handler);
#endif
            return 1;
        }();

        assert(wnd);

        int width, height;
        glfwGetFramebufferSize(wnd, &width, &height);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glViewport(0, 0, width, height);
    }

    void window::begin_draw()
    {
        glfwMakeContextCurrent(wnd);
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool window::should_close() const
    {
        return glfwWindowShouldClose(wnd);
    }

    void window::end_draw()
    {
        glfwSwapBuffers(wnd);
    }

    window::~window()
    {
        glfwDestroyWindow(wnd);
    }

    window::window(resolution res, const window& shared, const std::string& title)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        wnd = glfwCreateWindow(res.width, res.height, title.c_str(), nullptr, shared.wnd);

        glfwMakeContextCurrent(wnd);
        static auto init = [] {
            if (!gladLoadGL()) {
                throw std::runtime_error("could not initialize glad");
            }
#if defined(RTK_DEBUG)
            glad_set_post_callback(glad_post_handler);
#endif
            return 1;
        }();

        assert(wnd);

        int width, height;
        glfwGetFramebufferSize(wnd, &width, &height);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glViewport(0, 0, width, height);
    }

    static std::map<GLFWwindow*, window> windows;

    void window::input_callbacks()
    {
        ::glfwSetCursorPosCallback(wnd, [](GLFWwindow* w, double x, double y){

        });
    }

    void window::input_poll()
    {

    }

    bool window::get_key_down(int key) const
    {
        return glfwGetKey(wnd, key) == GLFW_PRESS;
    }

    bool window::get_key(int key) const
    {
        return glfwGetKey(wnd, key) == GLFW_REPEAT;
    }

    bool window::get_key_up(int key) const
    {
        return glfwGetKey(wnd, key) == GLFW_RELEASE;
    }

    glm::vec2 window::get_mouse() const
    {
        double x, y;
        glfwGetCursorPos(wnd, &x, &y);
        return {(float)x, (float)y};
    }

    void window::lock_cursor(bool lock)
    {
        glfwSetInputMode(wnd, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}


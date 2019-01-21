//
// Created by fatih on 19.03.2017.
//

#include <rtk/window.hpp>
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

        m_wnd = glfwCreateWindow(res.width, res.height, title.c_str(), nullptr, nullptr);

        glfwMakeContextCurrent(m_wnd);
        static auto init = [] {
            if (!gladLoadGL()) {
                throw std::runtime_error("could not initialize glad");
            }
#if defined(RTK_DEBUG)
            glad_set_post_callback(glad_post_handler);
#endif
            return 1;
        }();

        assert(m_wnd);

        set_viewport();
    }

    void window::begin_draw()
    {
        glfwMakeContextCurrent(m_wnd);
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool window::should_close() const
    {
        return glfwWindowShouldClose(m_wnd);
    }

    void window::end_draw()
    {
        glfwSwapBuffers(m_wnd);
    }

    window::~window()
    {
        glfwDestroyWindow(m_wnd);
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

        m_wnd = glfwCreateWindow(res.width, res.height, title.c_str(), nullptr, shared.m_wnd);

        glfwMakeContextCurrent(m_wnd);
        static auto init = [] {
            if (!gladLoadGL()) {
                throw std::runtime_error("could not initialize glad");
            }
#if defined(RTK_DEBUG)
            glad_set_post_callback(glad_post_handler);
#endif
            return 1;
        }();

        assert(m_wnd);

        set_viewport();
    }

    static std::map<GLFWwindow*, window> windows;

    void window::input_callbacks()
    {
        ::glfwSetCursorPosCallback(m_wnd, [](GLFWwindow* w, double x, double y){

        });
    }

    void window::input_poll()
    {

    }

    bool window::get_key_down(int key) const
    {
        return glfwGetKey(m_wnd, key) == GLFW_PRESS;
    }

    bool window::get_key(int key) const
    {
        return glfwGetKey(m_wnd, key) == GLFW_REPEAT;
    }

    bool window::get_key_up(int key) const
    {
        return glfwGetKey(m_wnd, key) == GLFW_RELEASE;
    }

    glm::vec2 window::get_mouse() const
    {
        double x, y;
        glfwGetCursorPos(m_wnd, &x, &y);
        return {(float)x, (float)y};
    }

    void window::lock_cursor(bool lock)
    {
        glfwSetInputMode(m_wnd, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    resolution window::get_resolution() const
    {
        int w, h;
        glfwGetFramebufferSize(m_wnd, &w, &h);
        return { rtk::pixels(w), rtk::pixels(h) };
    }

    void window::set_vsync(bool on)
    {
        use();
        glfwSwapInterval(on ? 1 : 0);
    }

    void window::set_viewport()
    {
        int width, height;
        glfwGetFramebufferSize(m_wnd, &width, &height);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glViewport(0, 0, width, height);
    }

    void window::use() const
    {
        glfwMakeContextCurrent(m_wnd);
    }
}


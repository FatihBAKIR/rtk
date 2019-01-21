//
// Created by fatih on 19.03.2017.
//

#pragma once

#include <rtk/graphics/size.hpp>
#include <rtk/rtk_fwd.hpp>
#include <GLFW/glfw3.h>
#include <rtk/display.hpp>
#include <string>
#include <array>
#include <glm/vec2.hpp>

namespace RTK_NAMESPACE {

    class RTK_PUBLIC window
    {
        GLFWwindow* m_wnd;

    public:
        window(resolution res, const std::string& title = std::string("rtk window"));
        window(resolution res, const window& shared, const std::string& title = std::string("rtk window"));

        void begin_draw();

        void end_draw();

        void input_callbacks();
        void input_poll();

        bool get_key_down(int key) const;
        bool get_key(int key) const;
        bool get_key_up(int key) const;
        glm::vec2 get_mouse() const;
        float get_scroll() const;

        void lock_cursor(bool lock = true);

        bool should_close() const;
        ~window();

        resolution get_resolution() const;

        void use() const;
        void set_vsync(bool on);

        void set_viewport();

        auto get() { return m_wnd; }
    private:

    };
}


//m
// Created by fatih on 1/29/19.
//

#include "cam_controller.hpp"
#include <glm/glm.hpp>
#include <rtk/window.hpp>
#include <rtk/imgui.h>

namespace app
{
    void cam_controller::pre_render(float dt) {
        auto camera_trans = m_cam->get_transform().get();

        auto cur = m_win->get_mouse();
        glm::vec2 diff = cur - m_cur_prev;
        m_cur_prev = cur;

        {
            diff /= 10.f;
            camera_trans->rotate(glm::vec3(diff.y, 0, 0));
            camera_trans->rotate(glm::vec3{ 0, -diff.x, 0 }, rtk::space::world);
        }

        glm::vec3 movement{};
        if (m_win->get_key_down(GLFW_KEY_S)) movement += rtk::vectors::back;
        if (m_win->get_key_down(GLFW_KEY_W)) movement += rtk::vectors::forward;
        if (m_win->get_key_down(GLFW_KEY_A)) movement += rtk::vectors::left;
        if (m_win->get_key_down(GLFW_KEY_D)) movement += rtk::vectors::right;

        normalize(movement);
        movement *= m_speed * dt * (m_win->get_key_down(GLFW_KEY_LEFT_SHIFT) * 2 + 1);
        camera_trans->translate(movement, rtk::space::self);
        //camera_trans->look_at({0, 0, 0});
        m_cam->sync();
    }

    cam_controller::cam_controller(std::unique_ptr<rtk::camera> cam, rtk::window &w)
            : m_win{&w}, m_cam(std::move(cam))
    {
            //m_cam->get_transform()->translate(rtk::vectors::up * 1.f);
            m_cur_prev = m_win->get_mouse();
            m_cam->get_transform()->translate(rtk::vectors::back * 2.f);
    }
}
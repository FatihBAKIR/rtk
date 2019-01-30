//
// Created by fatih on 1/29/19.
//

#include "cam_controller.hpp"
#include <glm/glm.hpp>
#include <rtk/window.hpp>

namespace app
{
    void cam_controller::pre_render(float dt) {
        auto camera_trans = m_cam->get_transform().get();
        glm::vec3 movement{};
        if (m_win->get_key_down(GLFW_KEY_S)) movement += rtk::vectors::back;
        if (m_win->get_key_down(GLFW_KEY_W)) movement += rtk::vectors::forward;
        if (m_win->get_key_down(GLFW_KEY_A)) movement += rtk::vectors::left * 2.f;
        if (m_win->get_key_down(GLFW_KEY_D)) movement += rtk::vectors::right * 2.f;
        normalize(movement);
        movement *= m_speed * dt;
        camera_trans->translate(movement);
        camera_trans->look_at({0,0,0});
        m_cam->sync();
    }

    cam_controller::cam_controller(std::unique_ptr<rtk::camera> cam, rtk::window &w)
            : m_win{&w}, m_cam(std::move(cam))
    {
            //m_cam->get_transform()->translate(rtk::vectors::up * 5.f);
            m_cam->get_transform()->translate(rtk::vectors::back * 2.f);
    }
}
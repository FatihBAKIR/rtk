//
// Created by fatih on 1/29/19.
//

#pragma once
#include <rtk/physics/transform.hpp>
#include <rtk/camera.hpp>

namespace app
{
    class cam_controller
    {
    public:
        void pre_render(float dt);

        void set_speed(float s)
        {
            m_speed = s;
        }

        cam_controller(std::unique_ptr<rtk::camera> cam, rtk::window& w);

        rtk::camera& get_camera() const
        {
            return *m_cam;
        }

    private:
        float m_speed = 1;
        rtk::window* m_win;
        std::unique_ptr<rtk::camera> m_cam;
    };
}
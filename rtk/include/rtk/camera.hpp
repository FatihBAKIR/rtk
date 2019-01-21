//
// Created by Mehmet Fatih BAKIR on 01/04/2017.
//

#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>
#include <rtk/physics/transform.hpp>
#include "framebuffer.hpp"
#include <rtk/display.hpp>
#include <rtk/rtk_fwd.hpp>

namespace RTK_NAMESPACE
{
    class RTK_PUBLIC camera
    {
        glm::mat4 m_view_matrix;
        glm::mat4 m_projection_matrix;
        glm::mat4 m_vp_matrix;

        glm::vec3 m_pos;

        glm::quat rotation;
        bool m_projection_dirty = true;

        float m_fov = 1.0471975512f; // 60 degrees
        float m_aspect_ratio = 1.333333f; //800 over 600
        float m_near_plane = 0.1f;
        float m_far_plane = 100.0f;

        glm::vec2 m_viewport_pos = {0.f, 0.f};
        glm::vec2 m_viewport_size = {1.f, 1.f};

        rtk::window* display = nullptr;

        const rtk::gl::framebuffer* m_fb = nullptr;

    public:
        camera(rtk::window& w)
            : m_trans{std::make_shared<rtk::transform>()}, display{&w}
        {
            reset_aspect_ratio();
        }

        const std::shared_ptr<rtk::transform>& get_transform() const {
            return m_trans;
        }

        void set_display(rtk::window& d)
        {
            display = &d;
            reset_aspect_ratio();
        }

        void sync();

        glm::mat4& get_vp_matrix();
        const glm::mat4& get_vp_matrix() const;

        void set_fov(float fov);

        /*
         * sets the aspect ratio from the display the camera is rendering to
         */
        void reset_aspect_ratio();
        void set_aspect_ratio(float aspect_ratio);
        void set_near_plane(float near_plane);
        void set_far_plane(float far_plane);

        void set_viewport(const glm::vec2& pos, const glm::vec2& sz);

        void render_to_texture(const rtk::gl::framebuffer& fb);

        void activate() const;

        void set_transform(std::shared_ptr<rtk::transform> trans)
        {
            m_trans = std::move(trans);
        }

    private:

        std::shared_ptr<rtk::transform> m_trans;
    };
}
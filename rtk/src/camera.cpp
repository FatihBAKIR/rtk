//
// Created by Mehmet Fatih BAKIR on 01/04/2017.
//

#include <rtk/camera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <rtk/utility.hpp>
#include <rtk/window.hpp>
#include <rtk/texture/tex2d.hpp>

namespace rtk
{
    void rtk::camera::sync() {
        auto camera_transform = m_trans.get();
        auto right = camera_transform->get_right();
        auto up = camera_transform->get_up();
        auto forward = camera_transform->get_forward();
        auto position = camera_transform->get_pos();

        m_view_matrix = glm::lookAt(position, position + forward, up);

        if (m_projection_dirty)
        {
            //m_projection_matrix = glm::ortho<float>(-2, 2, -2, 2, 0.1f, 10.f);

            m_projection_matrix = glm::perspective(m_fov, m_aspect_ratio, m_near_plane, m_far_plane);
            m_projection_dirty = false;
        }

        m_vp_matrix = m_projection_matrix * m_view_matrix;
    }
    void camera::set_fov(float fov)
    {
        m_fov = fov;
        m_projection_dirty = true;
    }

    void camera::set_aspect_ratio(float aspect_ratio)
    {
        m_aspect_ratio = aspect_ratio;
        m_projection_dirty = true;
    }

    void camera::set_near_plane(float near_plane)
    {
        m_near_plane = near_plane;
        m_projection_dirty = true;
    }

    void camera::set_far_plane(float far_plane)
    {
        m_far_plane = far_plane;
        m_projection_dirty = true;
    }

    glm::mat4 &camera::get_vp_matrix()
    {
        return m_vp_matrix;
    }

    const glm::mat4 &camera::get_vp_matrix() const
    {
        return m_vp_matrix;
    }

    void camera::reset_aspect_ratio()
    {
        auto res = display->get_resolution();
        float aspect = float(res.width) / res.height;
        set_aspect_ratio(aspect);
    }

    void camera::activate() const
    {
        auto reso = display->get_resolution();
        if (m_fb)
        {
            reso = m_fb->get_texture()->get_resolution();
            m_fb->activate();
        }
        else
        {
            rtk::set_viewport(rtk::get_rect(display->get_resolution(), m_viewport_pos, m_viewport_size));
        }
    }

    void camera::set_viewport(const glm::vec2& pos, const glm::vec2& sz)
    {
        m_viewport_pos = pos;
        m_viewport_size = sz;
    }

    void camera::render_to_texture(const gl::framebuffer& fb)
    {
        m_fb = &fb;
    }

    rtk::resolution camera::get_resolution() const {
        return display->get_resolution();
    }
}
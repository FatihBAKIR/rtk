//
// Created by fatih on 5/12/17.
//

#include <stdexcept>
#include <utility>
#include "rtk/framebuffer.hpp"
#include <rtk/texture/tex2d.hpp>
#include <rtk/framebuffer.hpp>


namespace RTK_NAMESPACE
{
namespace gl
{
    framebuffer::framebuffer(const texture2d& render_to)
    {
        glGenFramebuffers(1, &m_fb_id);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("error creating framebuffer");
        }
        m_render_to = &render_to;
    }

    framebuffer::framebuffer(framebuffer&& rhs)
    {
        m_fb_id = std::exchange(rhs.m_fb_id, 0);
    }

    framebuffer::~framebuffer()
    {
        if (m_fb_id != 0)
            glDeleteFramebuffers(1, &m_fb_id);
    }

    void framebuffer::activate() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fb_id);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_render_to->m_texture_id, 0);
    }

    void framebuffer::activate_col() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fb_id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_render_to->m_texture_id, 0);
    }


    bool framebuffer::complete() const {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    void framebuffer::activate_raw() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fb_id);
    }

    void framebuffer::activate_depth() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fb_id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_render_to->m_texture_id, 0);
    }

    void framebuffer::set_viewport() {
        glViewport(0, 0, m_render_to->get_resolution().width, m_render_to->get_resolution().height);
    }

    void reset_framebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
}

//
// Created by fatih on 5/12/17.
//

#pragma once

#include <glad/glad.h>
#include <rtk/rtk_fwd.hpp>

namespace RTK_NAMESPACE
{
namespace gl
{
    class RTK_PUBLIC framebuffer
    {
    public:
        explicit framebuffer(const rtk::gl::texture2d&);
        framebuffer(const framebuffer&) = delete;
        framebuffer(framebuffer&&);
        ~framebuffer();

        const texture2d* get_texture() const { return m_render_to; }

        void activate() const;
        void activate_depth() const;

        void set_viewport();

    private:
        const texture2d* m_render_to;
        GLuint m_fb_id;
    };

    RTK_PUBLIC void reset_framebuffer();
}
}




//
// Created by Mehmet Fatih BAKIR on 30/03/2017.
//

#pragma once

#include <rtk/rtk_fwd.hpp>
#include <glad/glad.h>
#include <cstddef>

namespace RTK_NAMESPACE
{
namespace gl
{
    class path
    {
        GLuint m_vao_id;
        GLuint m_vbo_id;

        std::size_t len;
        float width;

    public:
        path(const geometry::path&);
        void draw(program&) const;
    };
}
}


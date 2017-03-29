//
// Created by fatih on 27.03.2017.
//

#pragma once

#include <glad/glad.h>
#include <rtk/rtk_fwd.hpp>

namespace rtk
{
namespace gl
{
class mesh {
    GLuint m_vao_id = 0;
    GLuint m_pos_vbo_id = 0;
    GLuint m_col_vbo_id = 0;
    GLuint m_ebo_id = 0;

    long faces_len;
    long verts_len;

    void load(const geometry::mesh&);

public:
    mesh(const geometry::mesh& geomesh);
    ~mesh();

    void draw(gl::program& shader);
};
}
}


//
// Created by Mehmet Fatih BAKIR on 30/03/2017.
//

#include "path.hpp"
#include <rtk/geometry/path.hpp>
#include <rtk/gl/program.hpp>
#include <iostream>

namespace rtk
{
namespace gl
{
    path::path(const rtk::geometry::path& glp)
    {
        auto vert_span = glp.get_vertices();

        glGenVertexArrays(1, &m_vao_id);
        glGenBuffers(1, &m_vbo_id);

        glBindVertexArray(m_vao_id);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, vert_span.size_bytes(), vert_span.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vert_span.data()), (const void*) 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        len = vert_span.size();

        GLfloat lineWidthRange[2];
        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);

        std::cout << lineWidthRange[0] << ", " << lineWidthRange[1] << '\n';
    }

    void path::draw(program& shader) const
    {
        shader.use();

        glBindVertexArray(m_vao_id);
        glDrawArrays(GL_LINE_STRIP, 0, len);
    }
}
}

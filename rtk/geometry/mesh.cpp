//
// Created by fatih on 23.03.2017.
//

#include <glad/glad.h>
#include <rtk/gl/program.hpp>
#include <rtk/geometry/mesh.hpp>

namespace rtk
{
namespace geometry
{
    void mesh::set_vertices(gsl::span<const glm::vec3> vrts) {
        vertices_len = vrts.size();
        vertices = boost::shared_array<glm::vec3>(new glm::vec3[vertices_len]);
        std::copy(vrts.begin(), vrts.end(), vertices.get());
    }

    void mesh::set_faces(gsl::span<const std::uint32_t> fcs) {
        faces_len = fcs.size();
        faces = boost::shared_array<std::uint32_t>(new std::uint32_t[faces_len]);
        std::copy(fcs.begin(), fcs.end(), faces.get());
    }

    mesh::mesh() {
        faces_len = 0;
        vertices_len = 0;
    }

    void mesh::load() {
        glGenVertexArrays(1, &m_vao_id);
        glGenBuffers(1, &m_pos_vbo_id);
        glGenBuffers(1, &m_col_vbo_id);
        glGenBuffers(1, &m_ebo_id);

        glBindVertexArray(m_vao_id);

            glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo_id);
                glBufferData(GL_ARRAY_BUFFER, vertices_len * sizeof(*vertices.get()), vertices.get(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices.get()), (const void*)0);
                glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            if (colors)
            {
                glBindBuffer(GL_ARRAY_BUFFER, m_col_vbo_id);
                    glBufferData(GL_ARRAY_BUFFER, vertices_len * sizeof(*colors.get()), colors.get(), GL_STATIC_DRAW);

                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(*colors.get()), (const void*)0);
                    glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces_len * sizeof(*faces.get()), faces.get(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void mesh::draw(gl::program &shader) {
        shader.use();
        glBindVertexArray(m_vao_id);
        glDrawElements(GL_TRIANGLES, faces_len, GL_UNSIGNED_INT, 0);
    }

    void mesh::set_colors(gsl::span<const glm::vec3> cols) {
        Expects(cols.size() == vertices_len);
        colors = boost::shared_array<glm::vec3>(new glm::vec3[vertices_len]);
        std::copy(cols.begin(), cols.end(), colors.get());
    }
}
}


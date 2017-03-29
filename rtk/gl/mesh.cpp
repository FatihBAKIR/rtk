//
// Created by fatih on 27.03.2017.
//

#include <rtk/gl/mesh.hpp>
#include <rtk/geometry/mesh.hpp>
#include <rtk/gl/program.hpp>

namespace rtk {
    namespace gl {
        mesh::mesh(const geometry::mesh& geomesh)
        {
            load(geomesh);
        }

        void mesh::load(const geometry::mesh& geom)
        {
            auto vert_span = geom.get_vertices();
            auto face_span = geom.get_faces();
            auto cols_span = geom.get_colors();

            glGenVertexArrays(1, &m_vao_id);
            glGenBuffers(1, &m_pos_vbo_id);
            glGenBuffers(1, &m_ebo_id);

            glBindVertexArray(m_vao_id);

            glBindBuffer(GL_ARRAY_BUFFER, m_pos_vbo_id);
            glBufferData(GL_ARRAY_BUFFER, vert_span.size_bytes(), vert_span.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vert_span.data()), (const void*) 0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            if (!cols_span.empty()) {
                glGenBuffers(1, &m_col_vbo_id);
                glBindBuffer(GL_ARRAY_BUFFER, m_col_vbo_id);
                glBufferData(GL_ARRAY_BUFFER, cols_span.size_bytes(), cols_span.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(*cols_span.data()), (const void*) 0);
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_span.size_bytes(), face_span.data(), GL_STATIC_DRAW);

            glBindVertexArray(0);

            verts_len = vert_span.size();
            faces_len = face_span.size();
        }

        void mesh::draw(gl::program &shader) {
            shader.use();
            glBindVertexArray(m_vao_id);
            glDrawElements(GL_TRIANGLES, faces_len, GL_UNSIGNED_INT, 0);
        }

        mesh::~mesh()
        {
            glDeleteBuffers(1, &m_ebo_id);
            glDeleteBuffers(1, &m_pos_vbo_id);

            if (m_col_vbo_id)
            {
                glDeleteBuffers(1, &m_col_vbo_id);
            }

            glDeleteVertexArrays(1, &m_vao_id);
        }
    }
}

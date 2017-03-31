//
// Created by fatih on 27.03.2017.
//

#include <rtk/gl/mesh.hpp>
#include <rtk/geometry/mesh.hpp>
#include <rtk/gl/program.hpp>

namespace rtk {
    namespace gl {
        mesh::mesh(const rtk::geometry::mesh& geomesh)
        {
            load(geomesh);
        }

        void mesh::load(const rtk::geometry::mesh& geom)
        {
            auto vert_span = geom.get_vertices();
            auto face_span = geom.get_faces();

            glGenVertexArrays(1, &m_vao_id);
            glGenBuffers(1, &m_ebo_id);

            add_vertex_data(0, vert_span);

            glBindVertexArray(m_vao_id);
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

            for (const auto& vbo_id : vbos)
            {
                glDeleteBuffers(1, &vbo_id.second);
            }

            glDeleteVertexArrays(1, &m_vao_id);
        }

    }
}

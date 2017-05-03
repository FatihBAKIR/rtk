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
            auto uv_span = geom.get_uvs();

            glGenVertexArrays(1, &m_vao_id);
            glGenBuffers(1, &m_ebo_id);

            add_vertex_data(0, vert_span);
            add_vertex_data(2, uv_span);

            glBindVertexArray(m_vao_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_span.size_bytes(), face_span.data(), GL_STATIC_DRAW);
            glBindVertexArray(0);

            verts_len = vert_span.size();
            faces_len = face_span.size();
        }

        mesh::mesh(mesh&& rhs)
        {
            m_vao_id = std::exchange(rhs.m_vao_id, 0);
            m_ebo_id = std::exchange(rhs.m_ebo_id, 0);

            vbos = std::move(rhs.vbos);
            faces_len = rhs.faces_len;
            verts_len = rhs.verts_len;
        }

        void mesh::draw(const gl::program &shader) {
            shader.use();
            glBindVertexArray(m_vao_id);
            glDrawElements(GL_TRIANGLES, faces_len, GL_UNSIGNED_INT, 0);
        }

        mesh::~mesh()
        {
            if (m_vao_id == 0) return;

            glDeleteBuffers(1, &m_ebo_id);

            for (const auto& vbo_id : vbos)
            {
                glDeleteBuffers(1, &vbo_id.second);
            }

            glDeleteVertexArrays(1, &m_vao_id);
        }

        void mesh::swap(mesh&& rhs)
        {
            using std::swap;
            swap(m_vao_id, rhs.m_vao_id);
            swap(m_ebo_id, rhs.m_ebo_id);
            swap(vbos, rhs.vbos);
            swap(faces_len, rhs.faces_len);
            swap(verts_len, rhs.verts_len);
        }

        mesh& mesh::operator=(mesh rhs)
        {
            this->swap(std::move(rhs));
            return *this;
        }
    }
}

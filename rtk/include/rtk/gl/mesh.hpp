//
// Created by fatih on 27.03.2017.
//

#pragma once

#if defined(RTK_GLES2)
#include <GLES2/gl2.h>
#else
#endif

#include <glad/glad.h>
#include <rtk/rtk_fwd.hpp>
#include <map>
#include <glm/vec3.hpp>
#include <gsl/gsl>

namespace RTK_NAMESPACE {
    namespace gl
    {
        class mesh;
    }
    namespace geometry
    {
        RTK_PUBLIC gl::mesh create(const rtk::geometry::mesh&);
    }
    namespace gl {
        namespace detail
        {
            template<class>
            struct vb_traits;

            template<>
            struct vb_traits<glm::vec2>
            {
                static constexpr auto gl_type = GL_FLOAT;
                static constexpr auto gl_cnt = 2;
            };

            template<>
            struct vb_traits<glm::vec3>
            {
                static constexpr auto gl_type = GL_FLOAT;
                static constexpr auto gl_cnt = 3;
            };

            template <>
            struct vb_traits<glm::vec4>
            {
                static constexpr auto gl_type = GL_FLOAT;
                static constexpr auto gl_cnt = 4;
            };

            template<>
            struct vb_traits<float>
            {
                static constexpr auto gl_type = GL_FLOAT;
                static constexpr auto gl_cnt = 1;
            };

            template <>
            struct vb_traits<int>
            {
                static constexpr auto gl_type = GL_INT;
                static constexpr auto gl_cnt = 1;
            };
        }

        class RTK_PUBLIC mesh
        {
            GLuint m_vao_id = 0;
            GLuint m_ebo_id = 0;

            std::map<GLuint, GLuint> vbos;

            long faces_len;
            long verts_len;

            void load(const geometry::mesh&);

            mesh(const geometry::mesh& geomesh);

            friend gl::mesh geometry::create(const geometry::mesh&);
        public:
            mesh(const mesh&) = delete;
            mesh(mesh&& rhs);
            ~mesh();

            mesh& operator=(mesh);

            template <class T>
            void add_vertex_data(GLuint layout_id, gsl::span<const T>);

            void draw(const gl::program& shader);

            void swap(mesh&& rhs);
            friend void swap(mesh&& a, mesh&& b)
            {
                a.swap(std::move(b));
            }
        };

        template<class T>
        void mesh::add_vertex_data(GLuint layout_id, gsl::span<const T> data)
        {
            using traits = detail::vb_traits<T>;
            glGenBuffers(1, &vbos[layout_id]);

            glBindVertexArray(m_vao_id);

            glBindBuffer(GL_ARRAY_BUFFER, vbos[layout_id]);
            glBufferData(GL_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(layout_id, traits::gl_cnt, traits::gl_type, GL_FALSE, sizeof(*data.data()),
                    (const void*) 0);
            glEnableVertexAttribArray(layout_id);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }
}

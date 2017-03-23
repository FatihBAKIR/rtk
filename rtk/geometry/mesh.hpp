//
// Created by fatih on 23.03.2017.
//

#ifndef RENDERER_MESH_HPP
#define RENDERER_MESH_HPP

#include <gsl/gsl>
#include <glm/vec3.hpp>
#include <boost/shared_array.hpp>
#include <GL/gl.h>

namespace rtk
{
namespace geometry
{
    class mesh {
        GLuint m_vao_id;
        GLuint m_pos_vbo_id;
        GLuint m_col_vbo_id;
        GLuint m_ebo_id;

        std::size_t vertices_len;
        std::size_t faces_len;

        boost::shared_array<glm::vec3> vertices;
        boost::shared_array<glm::vec3> colors;
        boost::shared_array<std::uint32_t> faces;

    public:

        mesh();

        void set_vertices(gsl::span<const glm::vec3>);
        void set_colors(gsl::span<const glm::vec3>);
        void set_faces(gsl::span<const std::uint32_t>);

        void load();

        void draw(gl::program& shader);
    };
}
}


#endif //RENDERER_MESH_HPP

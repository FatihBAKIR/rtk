//
// Created by fatih on 23.03.2017.
//

#include <glad/glad.h>
#include <rtk/gl/program.hpp>
#include <rtk/geometry/mesh.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

namespace rtk
{
namespace geometry
{
    mesh::mesh() {
        faces_len = 0;
        vertices_len = 0;
    }

    void mesh::set_vertices(gsl::span<const glm::vec3> vrts) {
        vertices_len = vrts.size();
        vertices = boost::shared_array<glm::vec3>(new glm::vec3[vertices_len]);
        std::copy(vrts.begin(), vrts.end(), vertices.get());

        glm::vec3 min = vrts[0], max = vrts[0];
        auto v = vertices.get();
        for (auto i = vrts.begin(); i != vrts.end(); ++i, ++v)
        {
            *v = *i;
            min = glm::min(min, *i);
            max = glm::max(max, *i);
        }

        auto position = (max + min) * 0.5f;
        auto extent = max - min;
        auto _min = std::min({extent.x, extent.y, extent.z});
        auto _max = std::max({extent.x, extent.y, extent.z});

        auto scale = glm::scale(glm::vec3(1.f) / glm::vec3(_max));

	std::cout << position.x << ", " << position.y << ", " << position.z << '\n';
        for (auto v = vertices.get(); v != vertices.get() + vertices_len; ++v)
        {
            *v = scale * glm::vec4(*v - position, 1.f);
        }
    }

    void mesh::set_faces(gsl::span<const std::uint32_t> fcs) {
        faces_len = fcs.size();
        faces = boost::shared_array<std::uint32_t>(new std::uint32_t[faces_len]);
        std::copy(fcs.begin(), fcs.end(), faces.get());
    }

    void mesh::set_colors(gsl::span<const glm::vec3> cols) {
        Expects(cols.size() == vertices_len);
        colors = boost::shared_array<glm::vec3>(new glm::vec3[vertices_len]);
        std::copy(cols.begin(), cols.end(), colors.get());
    }
}
}


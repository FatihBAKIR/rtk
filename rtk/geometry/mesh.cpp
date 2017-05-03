//
// Created by fatih on 23.03.2017.
//

#include <glad/glad.h>
#include <rtk/gl/program.hpp>
#include <rtk/geometry/mesh.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <glm/gtx/euler_angles.hpp>
#include <rtk/physics/aabb.hpp>

namespace rtk
{
namespace geometry
{
    mesh::mesh() : bbox({}, {})
    {
        faces_len = 0;
        vertices_len = 0;
    }

    void mesh::set_vertices(gsl::span<const glm::vec3> vrts) {
        vertices_len = vrts.size();
        vertices = std::shared_ptr<glm::vec3>(new glm::vec3[vertices_len], [](auto* p){ delete[] p; });

        std::cout << vertices_len << '\n';
        glm::vec3 min = vrts[0], max = vrts[0];
        auto v = vertices.get();

        for (auto i = vrts.begin(); i != vrts.end(); ++i, ++v)
        {
            *v = *i;
            min = glm::min(min, *i);
            max = glm::max(max, *i);
        }

        bbox = physics::from_min_max(min, max);

        /*auto _min = std::min({bbox.extent.x, bbox.extent.y, bbox.extent.z});
        auto _max = std::max({bbox.extent.x, bbox.extent.y, bbox.extent.z});

        auto scale = glm::scale(glm::vec3(1.f) / glm::vec3(_max));
        auto rot = glm::eulerAngleYXZ(glm::radians(180.f), glm::radians(270.f), glm::radians(0.f));
        scale = rot * scale;

        for (auto v = vertices.get(); v != vertices.get() + vertices_len; ++v)
        {
            *v = scale * glm::vec4(*v - bbox.position, 1.f);
        }*/
    }

    void mesh::set_faces(gsl::span<const std::uint32_t> fcs) {
        faces_len = fcs.size();
        faces = std::shared_ptr<std::uint32_t>(new std::uint32_t[faces_len], [](auto* p) { delete[] p; });
        std::copy(fcs.begin(), fcs.end(), faces.get());
    }

    void mesh::set_uvs(gsl::span<const glm::vec2> p_uvs)
    {
        uvs = std::shared_ptr<glm::vec2>(new glm::vec2[vertices_len], [](auto* p){ delete[] p; });
        std::copy(p_uvs.begin(), p_uvs.end(), uvs.get());
    }
}
}


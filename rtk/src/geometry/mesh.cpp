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

        glm::vec3 min = vrts[0], max = vrts[0];
        auto v = vertices.get();

        for (auto i = vrts.begin(); i != vrts.end(); ++i, ++v)
        {
            *v = *i;
            min = glm::min(min, *i);
            max = glm::max(max, *i);
        }

        bbox = physics::from_min_max(min, max);
    }

    void mesh::set_faces(gsl::span<const std::uint32_t> fcs) {
        faces_len = fcs.size();
        faces = std::shared_ptr<std::uint32_t>(new std::uint32_t[faces_len], [](auto* p) { delete[] p; });
        std::copy(fcs.begin(), fcs.end(), faces.get());
    }

    void mesh::recalculate()
    {
        auto vrts = get_vertices();
        glm::vec3 min = vrts[0], max = vrts[0];

        for (auto i = vrts.begin(); i != vrts.end(); ++i)
        {
            min = glm::min(min, *i);
            max = glm::max(max, *i);
        }

        bbox = physics::from_min_max(min, max);
    }

    mesh primitive::quad() {
        std::array<glm::vec3, 4> vs = {
                glm::vec3{ -1, -1, 0 },
                glm::vec3{ 1, -1, 0 },
                glm::vec3{ 1, 1, 0 },
                glm::vec3{ -1, 1, 0 },
        };

        std::array<glm::vec2, 4> uvs = {
            glm::vec2{0, 0},
            glm::vec2{1, 0},
            glm::vec2{1, 1},
            glm::vec2{0, 1}
        };

        std::array<uint32_t, 2 * 3> faces {
            0, 1, 2,
            0, 2, 3
        };

        mesh m;
        m.set_vertices(vs);
        m.set_faces(faces);
        m.set_uvs(uvs);
        return m;
    }

    mesh primitive::cube()
    {
        std::array<glm::vec3, 8> vs = {
                glm::vec3{-.5f, -.5f, -.5f},
                glm::vec3{.5f, -.5f, -.5f},
                glm::vec3{.5f, -.5f, .5f},
                glm::vec3{-.5f, -.5f, .5f},
                glm::vec3{-.5f, .5f, -.5f},
                glm::vec3{.5f, .5f, -.5f},
                glm::vec3{.5f, .5f, .5f},
                glm::vec3{-.5f, .5f, .5f},
        };
        std::array<uint32_t, 36> faces = {
                0, 1, 2,
                0, 2, 3,
                0, 4, 1,
                4, 5, 1,
                1, 5, 2,
                5, 6, 2,
                6, 3, 2,
                6, 7, 3,
                4, 3, 7,
                4, 0, 3,
                5, 4, 7,
                5, 7, 6
        };

        mesh m;
        m.set_vertices(vs);
        m.set_faces(faces);
        return m;
    }

    void mesh::set_uvs(gsl::span<const glm::vec2> p_uvs)
    {
        uvs = std::shared_ptr<glm::vec2>(new glm::vec2[vertices_len], [](auto* p){ delete[] p; });
        std::copy(p_uvs.begin(), p_uvs.end(), uvs.get());
    }

    void mesh::set_normals(gsl::span<const glm::vec3> normals)
    {
        m_normals = std::shared_ptr<glm::vec3>(new glm::vec3[vertices_len], [](auto* p){ delete[] p; });
        std::copy(normals.begin(), normals.end(), m_normals.get());
    }
}
}


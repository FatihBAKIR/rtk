//
// Created by Mehmet Fatih BAKIR on 31/03/2017.
//

#include <rtk/mesh_ops.hpp>
#include <map>
#include <glm/glm.hpp>
#include <rtk/geometry/mesh.hpp>
#include <vector>

namespace rtk
{
namespace geometry
{
    std::vector<glm::vec3> generate_normals(const mesh& m)
    {
        auto get_area = [](int id)
        {
            return 1.f;
        };

        auto get_normal = [&](int id)
        {
            std::uint32_t v_ids[] = {
                m.get_faces()[id],
                m.get_faces()[id + 1],
                m.get_faces()[id + 2]
            };
            glm::vec3 vecs[3];
            std::transform(std::begin(v_ids), std::end(v_ids), std::begin(vecs), [&](int v_id)
            {
                return m.get_vertices()[v_id];
            });

            return glm::normalize(glm::cross(vecs[1] - vecs[0], vecs[2] - vecs[0]));
        };

        using map_t = std::map<long, std::vector<int>>;
        map_t vertex_tris;

        auto face_indices = m.get_faces();
        for (std::size_t i = 0; i < face_indices.size(); ++i) {
            int tri_index = i / 3;
            vertex_tris[face_indices[i]].push_back(tri_index);
        }

        std::map<long, glm::vec3> normals;

        for (auto& elem : vertex_tris)
        {
            normals[elem.first] = {};
            for (auto& tri : elem.second)
            {
                normals[elem.first] += get_normal(tri) * get_area(tri);
            }
            normals[elem.first] = glm::normalize(normals[elem.first]);
        }

        std::vector<glm::vec3> vert_normals(m.get_vertices().size());
        for (std::size_t i = 0; i < m.get_vertices().size(); ++i)
        {
            vert_normals[i] = normals[i];
        }
        return vert_normals;
    }
}
}
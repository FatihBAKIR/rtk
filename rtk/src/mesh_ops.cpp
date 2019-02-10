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
                m.get_faces()[id * 3],
                m.get_faces()[id * 3 + 1],
                m.get_faces()[id * 3 + 2]
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

    std::pair<std::vector<glm::vec3>, std::vector<glm::vec3>>
    generate_btn(const mesh & m) {
        auto get_bt = [&](int id)
        {
            std::uint32_t v_ids[] = {
                    m.get_faces()[id * 3],
                    m.get_faces()[id * 3 + 1],
                    m.get_faces()[id * 3 + 2]
            };

            glm::vec3 vecs[3];
            std::transform(std::begin(v_ids), std::end(v_ids), std::begin(vecs), [&](int v_id)
            {
                return m.get_vertices()[v_id];
            });

            glm::vec2 uvs[3];
            std::transform(std::begin(v_ids), std::end(v_ids), std::begin(uvs), [&](int v_id)
            {
                return m.get_uvs()[v_id];
            });

            auto edge1 = vecs[1] - vecs[0];
            auto edge2 = vecs[2] - vecs[0];

            auto deltaUV1 = uvs[1] - uvs[0];
            auto deltaUV2 = uvs[2] - uvs[0];

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            bt res;
            res.tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            res.tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            res.tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            res.tangent = glm::normalize(res.tangent);

            res.bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            res.bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            res.bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            res.bitangent = glm::normalize(res.bitangent);

            return res;
        };

        using map_t = std::map<long, std::vector<int>>;
        map_t vertex_tris;

        auto face_indices = m.get_faces();
        for (std::size_t i = 0; i < face_indices.size(); ++i) {
            int tri_index = i / 3;
            vertex_tris[face_indices[i]].push_back(tri_index);
        }

        std::map<long, bt> bts;

        for (auto& elem : vertex_tris)
        {
            bts[elem.first] = {};
            for (auto& tri : elem.second)
            {
                auto tbt = get_bt(tri);
                bts[elem.first].bitangent += tbt.bitangent;
                bts[elem.first].tangent += tbt.tangent;
            }
            bts[elem.first].bitangent = glm::normalize(bts[elem.first].bitangent);
            bts[elem.first].tangent = glm::normalize(bts[elem.first].tangent);
        }

        std::vector<glm::vec3> vert_bitangents(m.get_vertices().size());
        std::vector<glm::vec3> vert_tangents(m.get_vertices().size());
        for (std::size_t i = 0; i < m.get_vertices().size(); ++i)
        {
            vert_tangents[i] = bts[i].tangent;
            vert_bitangents[i] = bts[i].bitangent;
        }
        return {vert_bitangents, vert_tangents};
    }

    void transform(mesh& m, const glm::mat4& t)
    {
        for (auto& v : m.get_vertices())
        {
            v = glm::vec3(t * glm::vec4(v, 1.0f));
        }
        m.recalculate();
    }
}
}
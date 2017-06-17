//
// Created by fatih on 6/14/17.
//

#pragma once

#include <rtk/rtk_fwd.hpp>
#include <glm/vec3.hpp>
#include <vector>

class aiMesh;

namespace RTK_NAMESPACE
{
namespace assets
{
namespace assimp
{
    RTK_PUBLIC std::vector<glm::vec3> read_positions(const aiMesh* mesh);
    RTK_PUBLIC std::vector<std::uint32_t> read_faces(const aiMesh* mesh);
    RTK_PUBLIC std::vector<glm::vec2> read_uvs(const aiMesh* mesh);
}
}
}

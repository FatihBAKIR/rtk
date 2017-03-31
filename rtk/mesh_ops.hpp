//
// Created by Mehmet Fatih BAKIR on 31/03/2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <rtk/rtk_fwd.hpp>

namespace rtk
{
namespace geometry
{
    std::vector<glm::vec3> generate_normals(const mesh&);
}
}



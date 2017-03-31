//
// Created by Mehmet Fatih BAKIR on 30/03/2017.
//

#include "path.hpp"

namespace rtk
{
namespace geometry
{
    path::path()
    {
        size = 0;
    }

    void path::set_vertices(gsl::span<const glm::vec3> vrts)
    {
        size = vrts.size();
        vertices = boost::shared_array<glm::vec3>(new glm::vec3[size]);
        std::copy(vrts.begin(), vrts.end(), vertices.get());
    }

    gsl::span<const glm::vec3> path::get_vertices() const
    {
        return { vertices.get(), long(size) };
    }
}
}

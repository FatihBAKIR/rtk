//
// Created by Mehmet Fatih BAKIR on 26/03/2017.
//

#include <rtk/geometry/mesh.hpp>
#include <vector>

namespace RTK_NAMESPACE
{
namespace assets
{
    RTK_PUBLIC std::vector<geometry::mesh> load_meshes(const std::string& file);
    RTK_PUBLIC std::vector<geometry::mesh> load_meshes(const char* file_contents, size_t sz);
}
}

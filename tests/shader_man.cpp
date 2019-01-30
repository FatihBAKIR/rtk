//
// Created by fatih on 1/29/19.
//

#include "shader_man.hpp"
#include <rtk/gl/shader.hpp>
#include <rtk/gl/program.hpp>
#include <map>
#include <fstream>

namespace {

    std::string read_text_file(const std::string& path)
    {
        std::ifstream f(path);
        return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
    }

}
namespace app
{
    std::shared_ptr<rtk::gl::program> load_shader(const std::string& vert, const std::string frag)
    {
        static std::map<std::pair<std::string, std::string>, std::weak_ptr<rtk::gl::program>> cache;
        auto key = make_pair(vert, frag);
        if (auto it = cache.find(key); it != cache.end())
        {
            if (auto ptr = it->second.lock(); !ptr)
            {
                cache.erase(it);
            }
            else
            {
                return ptr;
            }
        }

        rtk::gl::vertex_shader mesh_vs { read_text_file(vert).c_str() };
        rtk::gl::fragment_shader mesh_fs { read_text_file(frag).c_str() };

        auto mesh_shader = std::make_shared<rtk::gl::program>();
        mesh_shader->attach(mesh_vs);
        mesh_shader->attach(mesh_fs);
        mesh_shader->link();

        cache[key] = mesh_shader;
        return mesh_shader;
    }
}
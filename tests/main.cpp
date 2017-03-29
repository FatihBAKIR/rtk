#include <iostream>

#include <rtk/rtk_init.hpp>
#include <rtk/window.hpp>

#include <rtk/gl/shader.hpp>
#include <rtk/geometry/mesh.hpp>
#include <fstream>
#include <rtk/gl/program.hpp>
#include <thread>

#include <rtk/asset/mesh_import.hpp>
#include <rtk/gl/mesh.hpp>

std::string read_text_file(const std::string& path)
{
    std::ifstream f(path);
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

int main() {
    rtk::rtk_init init;

    using namespace rtk::literals;
    using namespace std::chrono_literals;

    rtk::window w({800_px, 600_px});

    auto meshes = rtk::assets::load_meshes("../assets/teapot.obj");

    std::vector<rtk::gl::mesh> gl_meshes;
    gl_meshes.reserve(meshes.size());

    for (auto& m : meshes)
    {
        gl_meshes.emplace_back(m);
    }

    rtk::gl::vertex_shader vs { read_text_file("../shaders/basic.vert").c_str() };
    rtk::gl::fragment_shader fs { read_text_file("../shaders/basic.frag").c_str() };
    rtk::gl::program shader;
    shader.attach(vs);
    shader.attach(fs);
    shader.link();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!w.should_close())
    {
        w.begin_draw();

        for (auto& m : gl_meshes)
        {
            m.draw(shader);
        }

        w.end_draw();
        std::this_thread::sleep_for(10ms);
    }
}

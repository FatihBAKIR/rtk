#include <iostream>

#include <rtk/rtk_init.hpp>
#include <rtk/window.hpp>

#include <rtk/gl/shader.hpp>
#include <rtk/geometry/mesh.hpp>
#include <fstream>
#include <rtk/gl/program.hpp>
#include <thread>

#include <rtk/asset/mesh_import.hpp>

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

    rtk::gl::vertex_shader vs { read_text_file("../shaders/basic.vert").c_str() };
    rtk::gl::fragment_shader fs { read_text_file("../shaders/basic.frag").c_str() };
    rtk::gl::program shader;
    shader.attach(vs);
    shader.attach(fs);
    shader.link();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (true)
    {
        w.begin_draw();

        for (auto& m : meshes)
        {
            m.draw(shader);
        }

        w.end_draw();
        std::this_thread::sleep_for(10ms);
    }
}

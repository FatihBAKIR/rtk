#include <iostream>

#include <rtk/rtk_init.hpp>
#include <rtk/window.hpp>

#include <rtk/gl/shader.hpp>
#include <rtk/geometry/mesh.hpp>
#include <rtk/geometry/path.hpp>
#include <fstream>
#include <rtk/gl/program.hpp>
#include <thread>

#include <rtk/asset/mesh_import.hpp>
#include <rtk/gl/path.hpp>
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
    rtk::gl::geometry_shader gs { read_text_file("../shaders/line.geom").c_str() };
    rtk::gl::program shader;
    shader.attach(vs);
    shader.attach(gs);
    shader.attach(fs);
    shader.link();

    rtk::gl::program mesh_shader;
    mesh_shader.attach(vs);
    mesh_shader.attach(fs);
    mesh_shader.link();

    rtk::geometry::path p;
    p.set_vertices(std::vector<glm::vec3>{ meshes[0].get_vertices()[0], meshes[0].get_vertices()[1], meshes[0].get_vertices()[2] });

    rtk::gl::path gl_p (p);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!w.should_close())
    {
        w.begin_draw();

        for (auto& m : gl_meshes)
        {
            m.draw(mesh_shader);
        }

        gl_p.draw(shader);

        w.end_draw();
        std::this_thread::sleep_for(10ms);
    }
}

#include <iostream>

#include <rtk/graphics/units.hpp>
#include <rtk/graphics/size.hpp>
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
#include <rtk/mesh_ops.hpp>
#include <glm/gtx/transform.hpp>

std::string read_text_file(const std::string& path)
{
    std::ifstream f(path);
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

int main() {
    rtk::rtk_init init;

    using namespace rtk::literals;
    using namespace std::chrono_literals;

    rtk::window w({1600_px, 1600_px});

    auto meshes = rtk::assets::load_meshes("../assets/teapot.obj");

    std::vector<rtk::gl::mesh> gl_meshes;
    gl_meshes.reserve(meshes.size());

    for (auto& m : meshes)
    {
        gl_meshes.emplace_back(create(m));
    }

    rtk::gl::vertex_shader line_vs { read_text_file("../shaders/line.vert").c_str() };
    rtk::gl::fragment_shader line_fs { read_text_file("../shaders/line.frag").c_str() };
    rtk::gl::geometry_shader gs { read_text_file("../shaders/line.geom").c_str() };

    rtk::gl::vertex_shader mesh_vs { read_text_file("../shaders/basic.vert").c_str() };
    rtk::gl::fragment_shader mesh_fs { read_text_file("../shaders/basic.frag").c_str() };

    rtk::gl::program path_shader;
    path_shader.attach(line_vs);
    path_shader.attach(gs);
    path_shader.attach(line_fs);
    path_shader.link();
    path_shader.set_variable("color", glm::vec3(1.0f, 0.5f, 0.31f));

    rtk::gl::program mesh_shader;
    mesh_shader.attach(mesh_vs);
    mesh_shader.attach(mesh_fs);
    mesh_shader.link();

    auto& mesh = meshes[0];
    auto max = std::max({mesh.get_bbox().extent.x, mesh.get_bbox().extent.y, mesh.get_bbox().extent.z});
    auto model = glm::scale(glm::vec3(1.f, 1.f, 1.f) / glm::vec3(max, max, max)) * glm::translate(- mesh.get_bbox().position);

    mesh_shader.set_variable("model", model);
    mesh_shader.set_variable("view", glm::identity<glm::mat4>());
    mesh_shader.set_variable("projection", glm::identity<glm::mat4>());

    path_shader.set_variable("model", model);
    path_shader.set_variable("view", glm::identity<glm::mat4>());
    path_shader.set_variable("projection", glm::identity<glm::mat4>());

    rtk::geometry::path p;
    p.set_vertices(std::vector<glm::vec3>{ meshes[0].get_vertices()[0], meshes[0].get_vertices()[1], meshes[0].get_vertices()[2] });

    rtk::gl::path gl_p (p);

    auto normals = rtk::geometry::generate_normals(meshes[0]);
    gl_meshes[0].add_vertex_data<glm::vec3>(1, normals);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!w.should_close())
    {
        w.begin_draw();

        for (auto& m : gl_meshes)
        {
            m.draw(mesh_shader);
        }

        gl_p.draw(path_shader);

        w.end_draw();
        std::this_thread::sleep_for(10ms);
    }
}

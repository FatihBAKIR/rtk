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
#include <rtk/physics/transform.hpp>
#include <rtk/camera.hpp>

std::string read_text_file(const std::string& path)
{
    std::ifstream f(path);
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

rtk::gl::program load_shader()
{
    rtk::gl::vertex_shader mesh_vs { read_text_file("../shaders/basic.vert").c_str() };
    rtk::gl::fragment_shader mesh_fs { read_text_file("../shaders/basic.frag").c_str() };

    rtk::gl::program mesh_shader;
    mesh_shader.attach(mesh_vs);
    mesh_shader.attach(mesh_fs);
    mesh_shader.link();

    return mesh_shader;
}

int main() {
    rtk::rtk_init init;

    using namespace rtk::literals;
    using namespace std::chrono_literals;

    rtk::window w({800_px, 800_px});

    auto meshes = rtk::assets::load_meshes("../assets/teapot.obj");

    std::vector<rtk::gl::mesh> gl_meshes;
    gl_meshes.reserve(meshes.size());

    for (auto& m : meshes)
    {
        gl_meshes.emplace_back(create(m));

        auto normals = rtk::geometry::generate_normals(m);
        gl_meshes.back().add_vertex_data<glm::vec3>(1, normals);
    }

    auto& mesh = meshes[0];
    auto max = std::max({mesh.get_bbox().extent.x, mesh.get_bbox().extent.y, mesh.get_bbox().extent.z});

    auto transform = std::make_shared<rtk::transform>();
    transform->set_scale(glm::vec3(1.f, 1.f, 1.f) / glm::vec3(max, max, max));
    transform->set_position(-mesh.get_bbox().position / glm::vec3(max, max, max));

    auto mesh_shader = load_shader();

    auto camera_trans = std::make_shared<rtk::transform>();
    camera_trans->set_position(glm::vec3(0, 0, 0));
    auto cam = std::make_shared<rtk::camera>();
    cam->set_transform(camera_trans);
    cam->set_display(w);
    cam->reset_aspect_ratio();

    while (!w.should_close())
    {
        if (w.get_key_down(GLFW_KEY_S))
        {
            camera_trans->translate(glm::vec3{0, 0, -1} * 0.01f);
        }
        if (w.get_key_down(GLFW_KEY_W))
        {
            camera_trans->translate(glm::vec3{0, 0, 1} * 0.01f);
        }
        if (w.get_key_down(GLFW_KEY_A))
        {
            camera_trans->translate(glm::vec3{1, 0, 0} * 0.01f);
        }
        if (w.get_key_down(GLFW_KEY_D))
        {
            camera_trans->translate(glm::vec3{-1, 0, 0} * 0.01f);
        }

        transform->rotate(glm::vec3{0, 180, 0} * 0.01f);

        cam->sync();
        mesh_shader.set_variable("vp", cam->get_vp_matrix());

        w.begin_draw();

        cam->activate();
        for (auto& m : gl_meshes)
        {
            mesh_shader.set_variable("model", transform->get_world_mat4());
            m.draw(mesh_shader);
        }

        w.end_draw();
        std::this_thread::sleep_for(10ms);
    }
}

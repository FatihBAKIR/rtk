#include <iostream>
#include <chrono>

#include <rtk/imgui.h>
#include <rtk/geometry/mesh.hpp>
#include <rtk/rtk_init.hpp>
#include <rtk/asset/mesh_import.hpp>
#include <rtk/mesh_ops.hpp>
#include <rtk/texture/tex2d.hpp>

#include "material.hpp"
#include "lighting.hpp"
#include "scene.hpp"
#include "shader_man.hpp"
#include "cam_controller.hpp"
#include "imgui_glfw.hpp"

static std::shared_ptr<rtk::gl::program> get_phong_shader()
{
    return app::load_shader("../shaders/phong.vert", "../shaders/phong.frag");
}

struct phong_material : app::material
{
    rtk::gl::program& go() override
    {
        shader->set_variable("material.ambient", ambient);
        shader->set_variable("material.diffuse", diffuse);
        shader->set_variable("material.specular", specular);
        shader->set_variable("material.phong_exponent", phong_exponent);

        return *shader;
    }

    std::unique_ptr<material> clone() const override {
        auto res = std::make_unique<phong_material>();
        *res = *this;
        return res;
    }

    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
    float phong_exponent;
    std::shared_ptr<rtk::gl::program> shader;
};

auto get_phong_mat()
{
    auto mat = std::make_shared<phong_material>();
    mat->shader = get_phong_shader();
    mat->ambient = {1, 1, 1};
    mat->diffuse = {1, 1, 1};
    mat->specular = {1, 1, 1};
    mat->phong_exponent = 16.f;
    return mat;
}

auto get_ground(const std::shared_ptr<phong_material>& mat, const rtk::gl::mesh& mesh)
{
    app::renderable ground{};
    ground.name = "ground 2";
    ground.mat = mat->clone();
    auto ground_mat = dynamic_cast<phong_material*>(ground.mat.get());
    ground_mat->phong_exponent = 1;
    ground_mat->ambient = glm::vec3{2, 2, 2};
    ground.mesh = &mesh;
    ground.cast_shadow = false;

    ground.transform->translate(rtk::vectors::down * 0.5f);
    ground.transform->rotate({-90, 0, 0});
    ground.transform->set_scale({25, 25, 1});
    return ground;
}

namespace app {
    std::shared_ptr<rtk::gl::texture2d>
    render_to_tex(const rtk::camera &cam, const scene &ctx);

    std::shared_ptr<rtk::gl::texture2d>
    geometry_pass(const rtk::camera &cam, const scene &ctx);

    std::shared_ptr<rtk::gl::texture2d>
    object_pass(const rtk::camera &cam, const scene &ctx);

    std::shared_ptr<rtk::gl::texture2d>
    visualize_depth(const std::shared_ptr<rtk::gl::texture2d>& depth);

    std::shared_ptr<rtk::gl::texture2d>
    detect_edges(const std::shared_ptr<rtk::gl::texture2d>& depth);

    std::shared_ptr<rtk::gl::texture2d>
    render_tex(const std::shared_ptr<rtk::gl::texture2d>& depth);
    

    std::shared_ptr<rtk::gl::texture2d>
    thicker(const std::shared_ptr<rtk::gl::texture2d>& depth);

    std::shared_ptr<rtk::gl::texture2d>
    contour(const std::shared_ptr<rtk::gl::texture2d>& edges, const std::shared_ptr<rtk::gl::texture2d>& scene);
}

int main(int argc, char** argv) {
    using namespace rtk::literals;
    using namespace std::chrono_literals;
    using namespace app;

    rtk::rtk_init init;

    rtk::window w({1800_px, 1200_px});
    init_imgui(w);

    std::cout << "loading mesh from " << argv[1] << '\n';
    auto meshes = rtk::assets::load_meshes(argv[1]);
    meshes.push_back(rtk::geometry::primitive::cube());
    meshes.push_back(rtk::geometry::primitive::quad());

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

    auto mat = get_phong_mat();

    renderable teapot{};
    teapot.name = "teapot";
    teapot.mat = mat;
    teapot.mesh = &gl_meshes[0];

    teapot.transform->set_scale(glm::vec3(1.f, 1.f, 1.f) / glm::vec3(max, max, max));
    teapot.transform->set_position(-mesh.get_bbox().position / glm::vec3(max, max, max));

    auto scaled = mesh.get_bbox().extent / glm::vec3(max, max, max);

    renderable bounds{};
    bounds.name = "bounds";
    bounds.mat = mat->clone();
    auto bounds_mat = dynamic_cast<phong_material*>(bounds.mat.get());
    bounds_mat->diffuse = {0, 0, 0};
    bounds_mat->specular = {0, 0, 0};
    bounds.mesh = &gl_meshes[1];
    bounds.wire = true;
    bounds.cast_shadow = false;
    bounds.transform->set_parent(teapot.transform);
    bounds.transform->set_scale(mesh.get_bbox().extent);
    bounds.transform->set_position(mesh.get_bbox().position);

    teapot.transform->translate(rtk::vectors::down * (1 - scaled.y) * 0.5f);

    auto ground = get_ground(mat, gl_meshes[2]);

    spot_light pl;
    pl.color = glm::vec3{ 25, 0, 0 };
    pl.transform->set_position({ -5, 5, 0 });

    spot_light pl2;
    pl2.color = glm::vec3{ 0, 0, 25 };
    pl2.transform->set_position({ 5, 5, 0 });

    spot_light pl3;
    pl3.color = glm::vec3{ 0, 25, 0 };
    pl3.transform->set_position({ 0, 5, -5 });

    auto lights_p = std::make_shared<rtk::transform>();
    pl.transform->set_parent(lights_p);
    pl2.transform->set_parent(lights_p);
    pl3.transform->set_parent(lights_p);

    cam_controller cc{std::make_unique<rtk::camera>(w), w};

    scene ctx;
    ctx.objects.push_back(teapot);
    ctx.objects.push_back(ground);
    ctx.objects.push_back(bounds);

    ctx.lights.push_back(pl);
    ctx.lights.push_back(pl2);
    ctx.lights.push_back(pl3);
    ctx.ambient = ambient_light{ glm::vec3{ .1, .1, .1 } };

    using namespace std::chrono_literals;
    using clk = std::chrono::system_clock;
    std::chrono::microseconds dt = 10ms;
    for (auto& l : ctx.lights) l.transform->look_at(teapot.transform->get_pos());

    while (!w.should_close())
    {
        auto beg = clk::now();
        ImGui_ImplGlfwGL3_NewFrame();

        if (w.get_key_down(GLFW_KEY_L))
        {
            lights_p->rotate(glm::vec3{0, 1, 0});
        }

        auto obj_im = object_pass(cc.get_camera(), ctx);
        auto edges = detect_edges(obj_im);
        auto thicker = app::thicker(edges);
        auto thicker2 = app::thicker(thicker);
        auto thicker3 = app::thicker(thicker2);
        auto obj_v = visualize_depth(thicker3);

        auto out = render_to_tex(cc.get_camera(), ctx);

        auto fin = contour(thicker3, out);

        //auto im = geometry_pass(cc.get_camera(), ctx);
        //auto bet = visualize_depth(im);

        w.begin_draw();
        w.set_viewport();

        cc.pre_render(dt.count() / 1'000'000.f);

        //render(cc.get_camera(), ctx);

        ImGui::Begin("Info");
        ImGui::Text("FPS: %d", int(1000 / (dt.count() / 1'000.f)));
        ImGui::End();

        ImGui::Begin("Material");
        ImGui::ColorPicker3("Diffuse",
                dynamic_cast<phong_material*>(teapot.mat.get())->diffuse.data.data);
        ImGui::ColorPicker3("Specular",
                dynamic_cast<phong_material*>(teapot.mat.get())->specular.data.data);
        ImGui::End();

        ImGui::Begin("Shadow map of light 3");
        //ImGui::Image((void*)bet->get_id(), ImVec2{450,300});
        ImGui::Image((void*)fin->get_id(), ImVec2{900,600});
        ImGui::End();

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        w.end_draw();

        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(clk::now() - beg);
        dt = dur;
    }
}

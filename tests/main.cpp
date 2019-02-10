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

static std::shared_ptr<rtk::gl::program> get_skybox_shader()
{
    return app::load_shader("../shaders/skybox.vert", "../shaders/skybox.frag");
}

struct skybox_mat : app::material
{
    rtk::gl::program& go() override
    {
        shader->set_variable("skybox", *m_cm);
        shader->set_variable("tx", 0, *m_tx);
        return *shader;
    }

    std::unique_ptr<material> clone() const override {
        return std::unique_ptr<material>();
    }

    std::shared_ptr<rtk::gl::cubemap> m_cm;
    std::shared_ptr<rtk::gl::texture2d> m_tx;
    std::shared_ptr<rtk::gl::program> shader;
};

struct phong_material : app::material
{
    rtk::gl::program& go() override
    {
        shader->set_variable("material.ambient", ambient);
        shader->set_variable("material.diffuse", diffuse);
        shader->set_variable("material.specular", specular);
        shader->set_variable("material.phong_exponent", phong_exponent);

        shader->set_variable("material.textured", bool(tex));
        shader->set_variable("material.normaled", bool(normal_map));

        if (tex)
        {
            shader->set_variable("tex", 5, *tex);
            shader->set_variable("uv_scale", tex_scale);
        }

        if (normal_map)
        {
            shader->set_variable("normal_map", 6, *normal_map);
            shader->set_variable("uv_scale", tex_scale);
        }

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

    std::shared_ptr<rtk::gl::texture2d> normal_map;
    std::shared_ptr<rtk::gl::texture2d> tex;
    float tex_scale = 1;

    std::shared_ptr<rtk::gl::program> shader;
};

auto get_phong_mat()
{
    auto mat = std::make_shared<phong_material>();
    mat->shader = get_phong_shader();
    mat->ambient = {1, 1, 1};
    mat->diffuse = {1, 1, 1};
    mat->specular = {0, 0, 0};
    mat->phong_exponent = 4.f;
    return mat;
}

std::shared_ptr<rtk::gl::texture2d> load_tex(const std::string& path, bool mipmap = true)
{
    auto tex = rtk::graphics::load_texture(path);
    return std::make_shared<rtk::gl::texture2d>(tex, mipmap);
}

std::shared_ptr<rtk::gl::cubemap> load_cubemap(std::initializer_list<std::string> paths)
{
    std::vector<rtk::graphics::texture2d> texs;
    for (auto& s : paths)
    {
        texs.emplace_back(rtk::graphics::load_texture(s));
    }
    std::array<rtk::graphics::unsafe_texture*, 6> faces;
    std::transform(texs.begin(), texs.begin() + 6, faces.begin(), [](auto& x) {
        return &x;
    });
    return std::make_shared<rtk::gl::cubemap>(faces);
}

auto get_sky()
{
    return load_cubemap({
                                "../assets/skybox/ame_ash/ashcanyon_rt.png",
                                "../assets/skybox/ame_ash/ashcanyon_lf.png",
                                "../assets/skybox/ame_ash/ashcanyon_up.png",
                                "../assets/skybox/ame_ash/ashcanyon_dn.png",
                                "../assets/skybox/ame_ash/ashcanyon_ft.png",
                                "../assets/skybox/ame_ash/ashcanyon_bk.png",
    });
}

auto get_ground(const std::shared_ptr<phong_material>& mat, const rtk::gl::mesh& mesh)
{
    app::renderable ground{};
    ground.name = "ground 2";

    ground.mat = mat->clone();
    auto ground_mat = dynamic_cast<phong_material*>(ground.mat.get());
    ground_mat->tex = load_tex("../assets/textures/hardwood.png");
    ground_mat->tex_scale = 40;
    //ground_mat->normal_map = load_tex("../assets/textures/hardwood_normal.png", true);
    ground_mat->phong_exponent = 16;
    ground_mat->specular = glm::vec3{1, 1, 1};
    ground.mesh = &mesh;
    ground.cast_shadow = false;

    ground.transform->translate(rtk::vectors::down * 0.5f);
    ground.transform->rotate({-90, 0, 0});
    ground.transform->set_scale({25, 25, 1});
    return ground;
}

auto get_skybox_mat()
{
    auto mat = std::make_shared<skybox_mat>();
    mat->shader = get_skybox_shader();
    mat->m_cm = get_sky();
    mat->m_tx = load_tex("../assets/textures/hardwood.png");
    return mat;
}

auto get_skybox(const rtk::gl::mesh& mesh)
{
    app::renderable skybox{};
    skybox.name = "skybox";
    skybox.mesh = &mesh;
    skybox.cast_shadow = false;
    skybox.mat = get_skybox_mat();
    skybox.transform->set_scale({ 50, 50, 50 });
    return skybox;
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
    thicker(const std::shared_ptr<rtk::gl::texture2d>& depth);

    std::shared_ptr<rtk::gl::texture2d>
    contour(const std::shared_ptr<rtk::gl::texture2d>& edges, const std::shared_ptr<rtk::gl::texture2d>& scene);
}

int main(int argc, char** argv) {
    using namespace rtk::literals;
    using namespace std::chrono_literals;
    using namespace app;

    rtk::rtk_init init;

    rtk::window w({1440_px, 900_px});
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

        if (!m.has_normals())
        {
            auto normals = rtk::geometry::generate_normals(m);
            gl_meshes.back().add_vertex_data<glm::vec3>(1, normals);
        }

        if (!m.has_uvs())
            continue;

        auto [bitans, tans] = rtk::geometry::generate_btn(m);
        gl_meshes.back().add_vertex_data<glm::vec3>(3, bitans);
        gl_meshes.back().add_vertex_data<glm::vec3>(4, tans);
    }

    auto& mesh = meshes[0];
    auto max = std::max({mesh.get_bbox().extent.x, mesh.get_bbox().extent.y, mesh.get_bbox().extent.z});

    auto mat = get_phong_mat();

    renderable teapot{};
    teapot.name = "teapot";
    teapot.mat = mat;
    teapot.mesh = &gl_meshes[0];

    //teapot.transform->rotate(glm::vec3(-90, 0, 0));

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

    //auto sky = get_skybox(gl_meshes[1]);

    auto tp_mat = dynamic_cast<phong_material*>(teapot.mat.get());
    tp_mat->tex = load_tex("../assets/WoodenCrate/Texture/WoodenCrate_Albedo.png");
    tp_mat->normal_map = load_tex("../assets/WoodenCrate/Texture/WoodenCrate_Normal.png", false);
    //tp_mat->diffuse *= 2.f;

    area_light pl;
    pl.color = glm::vec3{ 25, 25, 25 };
    pl.transform->set_position({ 5, 5, -2 });

    area_light pl2;
    pl2.color = glm::vec3{ 25, 25, 25 };
    pl2.transform->set_position({ 0, 5, 5 });

    area_light pl3;
    pl3.color = glm::vec3{ 25, 25, 25 };
    pl3.transform->set_position({ -5, 5, 0 });

    area_light pl4;
    pl4.color = glm::vec3{ 25, 25, 25 };
    pl4.transform->set_position({ 5, 5, 0 });

    cam_controller cc{std::make_unique<rtk::camera>(w), w};

    scene ctx;
    ctx.objects.push_back(teapot);
    ctx.objects.push_back(ground);
    //ctx.objects.push_back(bounds);
    //ctx.objects.push_back(sky);

    ctx.lights.push_back(pl);
    //ctx.lights.push_back(pl2);
    //ctx.lights.push_back(pl3);
    //ctx.lights.push_back(pl4);
    ctx.ambient = ambient_light{ glm::vec3{ .2, .2, .2 } };

    for (auto& l : ctx.lights) l.transform->look_at(teapot.transform->get_pos());
    using namespace std::chrono_literals;
    using clk = std::chrono::system_clock;
    std::chrono::microseconds dt = 10ms;

    while (!w.should_close())
    {
        auto beg = clk::now();
        ImGui_ImplGlfwGL3_NewFrame();

        if (w.get_key_down(GLFW_KEY_UP)){
            pl.transform->translate(rtk::vectors::forward / 50.f, rtk::space::world);

            for (auto& l : ctx.lights) l.transform->look_at(teapot.transform->get_pos());
        }
        if (w.get_key_down(GLFW_KEY_DOWN)){
            pl.transform->translate(rtk::vectors::back / 50.f, rtk::space::world);

            for (auto& l : ctx.lights) l.transform->look_at(teapot.transform->get_pos());
        }

        if (w.get_key_down(GLFW_KEY_LEFT)){
            pl.transform->translate(rtk::vectors::left / 50.f, rtk::space::world);

            for (auto& l : ctx.lights) l.transform->look_at(teapot.transform->get_pos());
        }
        if (w.get_key_down(GLFW_KEY_RIGHT)){
            pl.transform->translate(rtk::vectors::right / 50.f, rtk::space::world);

            for (auto& l : ctx.lights) l.transform->look_at(teapot.transform->get_pos());
        }


        if (w.get_key_down(GLFW_KEY_P)){
            pl.transform->translate(rtk::vectors::up / 50.f, rtk::space::world);

            for (auto& l : ctx.lights) l.transform->look_at(teapot.transform->get_pos());
        }
        if (w.get_key_down(GLFW_KEY_L)){
            pl.transform->translate(rtk::vectors::down / 50.f, rtk::space::world);

            for (auto& l : ctx.lights) l.transform->look_at(teapot.transform->get_pos());
        }

        //auto obj_im = object_pass(cc.get_camera(), ctx);
        //auto edges = detect_edges(obj_im);
        //auto thicker = app::thicker(edges);
        //auto thicker2 = app::thicker(thicker);
        //auto thicker3 = app::thicker(thicker2);
        //auto obj_v = visualize_depth(thicker3);

        //auto out = render_to_tex(cc.get_camera(), ctx);

        //auto fin = contour(thicker, out);

        //auto im = geometry_pass(cc.get_camera(), ctx);
        //auto bet = visualize_depth(im);

        w.begin_draw();
        w.set_viewport();

        cc.pre_render(dt.count() / 1'000'000.f);

        render(cc.get_camera(), ctx);

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
        //ImGui::Image((void*)thicker->get_id(), ImVec2{900,600});
        ImGui::SliderFloat("Light size", &ctx.lights[0].size, 1, 10);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        w.end_draw();

        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(clk::now() - beg);
        dt = dur;
    }
}

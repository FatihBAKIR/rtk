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
#include <rtk/texture/tex2d.hpp>

#include <rtk/imgui.h>
#include "imgui_glfw.hpp"

std::string read_text_file(const std::string& path)
{
    std::ifstream f(path);
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

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

std::shared_ptr<rtk::gl::program> get_phong_shader()
{
    return load_shader("../shaders/phong.vert", "../shaders/phong.frag");
}

std::shared_ptr<rtk::gl::program> get_shadow_shader()
{
    return load_shader("../shaders/shadow.vert", "../shaders/shadow.frag");
}

class cam_controller
{
public:
    void pre_render(float dt)
    {
        auto camera_trans = m_cam->get_transform().get();
        glm::vec3 movement{};
        if (m_win->get_key_down(GLFW_KEY_S)) movement += rtk::vectors::back;
        if (m_win->get_key_down(GLFW_KEY_W)) movement += rtk::vectors::forward;
        if (m_win->get_key_down(GLFW_KEY_A)) movement += rtk::vectors::left * 2.f;
        if (m_win->get_key_down(GLFW_KEY_D)) movement += rtk::vectors::right * 2.f;
        normalize(movement);
        movement *= m_speed * dt;
        camera_trans->translate(movement);
        camera_trans->look_at({0,0,0});
        m_cam->sync();
    }

    void set_speed(float s)
    {
        m_speed = s;
    }

    cam_controller(std::unique_ptr<rtk::camera> cam, rtk::window& w)
        : m_win{&w}, m_cam(std::move(cam))
    {
        m_cam->get_transform()->translate(rtk::vectors::back * 2.f);
    }

    rtk::camera& get_camera() const
    {
        return *m_cam;
    }

private:
    float m_speed = 1;
    rtk::window* m_win;
    std::unique_ptr<rtk::camera> m_cam;
};

struct spot_light
{
    glm::vec3 color;
    std::shared_ptr<rtk::transform> transform = std::make_shared<rtk::transform>();
};

struct material
{
public:
    virtual rtk::gl::program& go() = 0;
    virtual std::unique_ptr<material> clone() const = 0;
    virtual ~material() = default;
};

struct phong_material : material
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

struct renderable
{
    std::string name;
    rtk::gl::mesh* mesh;
    std::shared_ptr<rtk::transform> transform = std::make_shared<rtk::transform>();
    std::shared_ptr<material> mat;
    bool wire = false;
    bool cast_shadow = true;
};

struct ambient_light
{
    glm::vec3 ambient;
};

struct scene
{
    std::vector<renderable> objects;
    std::vector<spot_light> lights;
    ambient_light ambient;
};

void apply(rtk::gl::program& p, const std::string& base, const spot_light& pl)
{
    p.set_variable(base + ".intensity", pl.color);
    p.set_variable(base + ".position", pl.transform->get_pos());
}

void apply(rtk::gl::program& p, const ambient_light& al)
{
    p.set_variable("ambient_light", al.ambient);
}

auto light_pass(const scene& ctx, const spot_light& l)
{
    using namespace rtk::literals;
    auto out = rtk::gl::create_texture(
            rtk::resolution(4096_px, 4096_px),
            rtk::graphics::pixel_format::gl_depth16);
    out->activate(0);
    rtk::gl::framebuffer shadow_buf(*out);

    static auto shader = get_shadow_shader();

    const glm::mat4 dpm = glm::ortho<float>(-5, 5, -5, 5, 0.1f, 10.f);

    auto& trans = l.transform;
    auto pos = trans->get_pos();
    auto dvm = glm::lookAt(
            pos,
            pos + trans->get_forward(),
            trans->get_up());

    const auto mvp = dpm * dvm;

    shader->set_variable("vp", mvp);
    shadow_buf.activate_depth();
    shadow_buf.set_viewport();

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (auto& obj : ctx.objects)
    {
        if (!obj.cast_shadow)
        {
            continue;
        }
        shader->set_variable("model", obj.transform->get_world_mat4());
        obj.mesh->draw(*shader);
    }

    return make_pair(out, mvp);
}

struct shadow_ctx
{
    std::vector<std::shared_ptr<rtk::gl::texture2d>> sms;
    std::vector<glm::mat4> light_transforms;
};

void render_one(
        const rtk::camera& cam,
        const scene& ctx,
        const shadow_ctx& shadow,
        const renderable& obj)
{
    auto mat = obj.mat.get();
    auto& shader = mat->go();

    shader.set_variable("camera_position", cam.get_transform()->get_pos());
    shader.set_variable("vp", cam.get_vp_matrix());
    shader.set_variable("model", obj.transform->get_world_mat4());

    int pl_num = 0;
    for (auto& pl : ctx.lights)
    {
        apply(shader, "point_light[" + std::to_string(pl_num) + "]", pl);

        shader.set_variable("point_light[" + std::to_string(pl_num) + "].shadowTex", pl_num, *shadow.sms[pl_num]);
        shader.set_variable("point_light[" + std::to_string(pl_num) + "].transform", shadow.light_transforms[pl_num]);

        pl_num++;
    }

    shader.set_variable("number_of_point_lights", pl_num);

    apply(shader, ctx.ambient);

    cam.activate();
    if (obj.wire)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    obj.mesh->draw(shader);
    if (obj.wire)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

std::shared_ptr<rtk::gl::texture2d> sm;
void render(const rtk::camera& cam, const scene& ctx)
{
    shadow_ctx shadow;

    glCullFace(GL_FRONT);
    for (auto& light : ctx.lights)
    {
        auto [out, vp] = light_pass(ctx, light);
        shadow.sms.push_back(out);
        shadow.light_transforms.push_back(vp);
        sm = out;
    }
    glCullFace(GL_BACK);

    rtk::gl::reset_framebuffer();

    for (auto& obj : ctx.objects)
    {
        render_one(cam, ctx, shadow, obj);
    }
}

void init_imgui(rtk::window& win)
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfwGL3_Init(win.get(), true);

    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.WindowRounding = 0;
}

int main(int argc, char** argv) {
    using namespace rtk::literals;
    using namespace std::chrono_literals;

    std::cout << "loading mesh from " << argv[1] << '\n';
    auto meshes = rtk::assets::load_meshes(argv[1]);
    meshes.push_back(rtk::geometry::primitive::cube());

    rtk::rtk_init init;

    rtk::window w({1920_px, 1080_px});
    init_imgui(w);

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

    auto mat = std::make_shared<phong_material>();
    mat->shader = get_phong_shader();
    mat->ambient = {1, 1, 1};
    mat->diffuse = {1, 1, 1};
    mat->specular = {1, 1, 1};
    mat->phong_exponent = 1.f;

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

    renderable ground{};
    ground.name = "ground 2";
    ground.mat = mat;
    ground.mesh = &gl_meshes[1];
    ground.cast_shadow = false;

    ground.transform->translate(rtk::vectors::down);
    ground.transform->set_scale({25, 1, 25});

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

    scene ctx;
    ctx.objects.push_back(teapot);
    ctx.objects.push_back(ground);
    ctx.objects.push_back(bounds);
    ctx.lights.push_back(pl);
    ctx.lights.push_back(pl2);
    ctx.lights.push_back(pl3);
    ctx.ambient = ambient_light{ glm::vec3{ .1, .1, .1 } };

    cam_controller cc{std::make_unique<rtk::camera>(w), w};

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
            auto pos = ctx.lights[0].transform->get_pos();
            std::cout << pos.x << ", " << pos.y << ", " << pos.z << '\n';
        }

        w.begin_draw();
        w.set_viewport();

        cc.pre_render(dt.count() / 1'000'000.f);

        render(cc.get_camera(), ctx);

        ImGui::Begin("About");
        ImGui::Text("FPS: %d", int(1000 / (dt.count() / 1'000.f)));
        ImGui::ImageButton((void*)sm->get_id(), ImVec2{256,256});
        ImGui::End();

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        w.end_draw();

        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(clk::now() - beg);
        dt = dur;
    }
}

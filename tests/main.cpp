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

#include <opencv2/opencv.hpp>

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
    return load_shader("../shaders/basic.vert", "../shaders/basic.frag");
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
        if (m_win->get_key_down(GLFW_KEY_A)) movement += rtk::vectors::left;
        if (m_win->get_key_down(GLFW_KEY_D)) movement += rtk::vectors::right;
        normalize(movement);
        movement *= m_speed * dt;
        camera_trans->translate(movement);
        m_cam->sync();
        m_cam->activate();
    }

    void set_speed(float s)
    {
        m_speed = s;
    }

    cam_controller(std::unique_ptr<rtk::camera> cam, rtk::window& w)
        : m_win{&w}, m_cam(std::move(cam))
    {
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

struct point_light
{
    glm::vec3 color;
    rtk::transform transform;
};

struct material
{
public:
    virtual rtk::gl::program& go() = 0;
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
};

struct ambient_light
{
    glm::vec3 ambient;
};

struct render_ctx
{
    std::vector<renderable> objects;
    std::vector<point_light> lights;
    std::vector<std::shared_ptr<rtk::gl::texture2d>> sms;
    ambient_light ambient;
};

struct light_info
{
    point_light pl;
    rtk::gl::texture2d sm;
};

void apply(rtk::gl::program& p, const std::string& base, const point_light& pl)
{
    p.set_variable(base + ".intensity", pl.color);
    p.set_variable(base + ".position", pl.transform.get_pos());
}

void apply(rtk::gl::program& p, const ambient_light& al)
{
    p.set_variable("ambient_light", al.ambient);
}

struct shadow_ctx
{
};

const glm::mat4 dpm = glm::ortho<float>(-10, 10, -10, 10, 0.1f, 5.f);

auto light_pass(render_ctx& ctx, point_light& l)
{
    using namespace rtk::literals;
    auto out = rtk::gl::create_texture(
            rtk::resolution(1024_px, 1024_px),
            rtk::graphics::pixel_format::gl_depth16);
    out->activate(0);
    auto shadow_buf = new rtk::gl::framebuffer(*out);

    static auto shader = get_shadow_shader();

    auto trans = &l.transform;
    auto dvm = glm::lookAt(
            trans->get_pos(),
            trans->get_pos() + trans->get_forward(),
            trans->get_up());
    const auto mvp = dpm * dvm;

    shader->set_variable("vp", mvp);

    glViewport(0, 0, out->get_resolution().width, out->get_resolution().height);
    shadow_buf->activate_depth();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (auto& obj : ctx.objects)
    {
        std::cout << "shadow " << obj.name << '\n';
        shader->set_variable("model", obj.transform->get_world_mat4());
        obj.mesh->draw(*shader);
    }

    auto dep = read_depth(*out);

    cv::Mat m(dep.m_height, dep.m_width, CV_32FC1, dep.m_data);
    cv::imshow("asd", m);
    cv::waitKey(0);

    return out;
}

void render_one(rtk::camera& cam, render_ctx& ctx, renderable& obj)
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
        pl_num++;
    }
    shader.set_variable("number_of_point_lights", pl_num);
    shader.set_variable("shadowTex", 0);
    auto trans = &ctx.lights[0].transform;

    auto dvm = glm::lookAt(
            trans->get_pos(),
            trans->get_pos() + trans->get_forward(),
            trans->get_up());

    const auto mvp = dpm * dvm;

    shader.set_variable("lightMat", mvp);

    apply(shader, ctx.ambient);

    ctx.sms[0]->activate(0);
    obj.mesh->draw(shader);
}

void render(rtk::camera& cam, render_ctx& ctx)
{
    for (auto& obj : ctx.objects)
    {
        render_one(cam, ctx, obj);
    }
}

int main() {
    rtk::rtk_init init;

    using namespace rtk::literals;
    using namespace std::chrono_literals;

    rtk::window w({1920_px, 1200_px});

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

    point_light pl;
    pl.color = glm::vec3{ 200, 20, 20 } / 4.f;
    pl.transform.set_position({ -1, 2, 0 });
    pl.transform.look_at(teapot.transform->get_pos());

    render_ctx ctx;
    ctx.objects.push_back(teapot);
    ctx.lights.push_back(pl);
    ctx.ambient = ambient_light{ glm::vec3{ .1, .1, .1 } };
    ctx.sms.resize(1);

    cam_controller cc{std::make_unique<rtk::camera>(w), w};

    using namespace std::chrono_literals;
    using clk = std::chrono::system_clock;
    std::chrono::microseconds dt = 10ms;
    while (!w.should_close())
    {
        auto beg = clk::now();
        w.begin_draw();

        auto out = light_pass(ctx, ctx.lights[0]);
        ctx.sms[0] = out;

        rtk::gl::reset_framebuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cc.pre_render(dt.count() / 1'000'000.f);

        render(cc.get_camera(), ctx);

        w.end_draw();

        while (true);
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(clk::now() - beg);
        std::cout << dur.count() << '\n';
        std::this_thread::sleep_for(10ms - dt);
        dt = 10ms;
    }
}

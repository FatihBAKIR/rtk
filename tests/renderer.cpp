//
// Created by fatih on 1/29/19.
//

#include "renderable.hpp"
#include "lighting.hpp"
#include "scene.hpp"
#include <rtk/camera.hpp>

#include <rtk/texture/tex2d.hpp>
#include <rtk/framebuffer.hpp>
#include "shader_man.hpp"
#include <rtk/geometry/mesh.hpp>
#include <rtk/utility.hpp>
#include <rtk/graphics/rect.hpp>

namespace {
    std::shared_ptr<rtk::gl::program> get_shadow_shader()
    {
        return app::load_shader("../shaders/only_pos.vert", "../shaders/shadow.frag");
    }

    std::shared_ptr<rtk::gl::program> get_geometry_shader()
    {
        return app::load_shader("../shaders/only_pos.vert", "../shaders/geometry.frag");
    }

    std::shared_ptr<rtk::gl::program> get_object_shader()
    {
        return app::load_shader("../shaders/only_pos.vert", "../shaders/object.frag");
    }

    std::shared_ptr<rtk::gl::program> get_edge_detect_shader()
    {
        return app::load_shader("../shaders/full_tex.vert", "../shaders/edge_detect.frag");
    }

    std::shared_ptr<rtk::gl::program> get_thicker_shader()
    {
        return app::load_shader("../shaders/full_tex.vert", "../shaders/thicker.frag");
    }

    std::shared_ptr<rtk::gl::program> get_contour_shader()
    {
        return app::load_shader("../shaders/full_tex.vert", "../shaders/contour.frag");
    }

    std::shared_ptr<rtk::gl::program> get_tex_shader()
    {
        return app::load_shader("../shaders/full_tex.vert", "../shaders/draw_tex.frag");
    }

    std::shared_ptr<rtk::gl::program> get_overlay_shader()
    {
        return app::load_shader("../shaders/full_tex.vert", "../shaders/overlay.frag");
    }

    std::shared_ptr<rtk::gl::program> get_depth_vis_shader()
    {
        return app::load_shader("../shaders/full_tex.vert", "../shaders/depth_vis.frag");
    }
}

namespace app
{
    std::pair<std::shared_ptr<rtk::gl::texture2d>, glm::mat4>
    light_pass(const scene& ctx, const area_light& l, std::shared_ptr<rtk::gl::texture2d>& tex)
    {
        using namespace rtk::literals;

        rtk::gl::framebuffer shadow_buf(*tex);

        static auto shader = get_shadow_shader();

        const glm::mat4 dpm = glm::perspective<float>(glm::radians(90.f), 1, 1.f, 100.f);
        //const glm::mat4 dpm = glm::ortho<float>(-2, 2, -2, 2, 0.1f, 10.f);

        auto& trans = l.transform;
        auto pos = trans->get_pos();
        auto fwd = trans->get_forward();
        auto up = trans->get_up();

        const auto dvm = glm::lookAt(
                pos,
                pos + fwd,
                up);

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

        return { tex, mvp };
    }

    std::shared_ptr<rtk::gl::texture2d>
    visualize_depth(const std::shared_ptr<rtk::gl::texture2d>& depth)
    {
        using namespace rtk::literals;
        auto out = rtk::gl::create_texture(
                depth->get_resolution(),
                rtk::graphics::pixel_format::rgb_byte);
        out->activate(0);
        rtk::gl::framebuffer shadow_buf(*out);

        auto mesh = create(rtk::geometry::primitive::quad());

        auto vis_shader = get_depth_vis_shader();

        shadow_buf.activate();
        shadow_buf.set_viewport();

        glClear(GL_COLOR_BUFFER_BIT);

        vis_shader->set_variable("d", 0, *depth);

        mesh.draw(*vis_shader);

        return out;
    }

    std::shared_ptr<rtk::gl::texture2d>
    detect_edges(const std::shared_ptr<rtk::gl::texture2d>& depth)
    {
        using namespace rtk::literals;
        static auto out = rtk::gl::create_texture(
                depth->get_resolution(),
                rtk::graphics::pixel_format::rgb_byte);

        static rtk::gl::framebuffer shadow_buf(*out);

        auto mesh = create(rtk::geometry::primitive::quad());

        auto vis_shader = get_edge_detect_shader();

        shadow_buf.activate();
        shadow_buf.set_viewport();

        glClear(GL_COLOR_BUFFER_BIT);

        vis_shader->set_variable("d", 0, *depth);

        mesh.draw(*vis_shader);

        rtk::gl::reset_framebuffer();

        return out;
    }

    std::shared_ptr<rtk::gl::texture2d>
    thicker(const std::shared_ptr<rtk::gl::texture2d>& depth)
    {
        using namespace rtk::literals;
        static auto out = rtk::gl::create_texture(
                depth->get_resolution(),
                rtk::graphics::pixel_format::rgb_byte);

        static rtk::gl::framebuffer shadow_buf(*out);

        auto mesh = create(rtk::geometry::primitive::quad());

        auto vis_shader = get_thicker_shader();

        shadow_buf.activate();
        shadow_buf.set_viewport();

        glClear(GL_COLOR_BUFFER_BIT);

        vis_shader->set_variable("d", 0, *depth);

        mesh.draw(*vis_shader);

        rtk::gl::reset_framebuffer();

        return out;
    }

    std::shared_ptr<rtk::gl::texture2d>
    contour(const std::shared_ptr<rtk::gl::texture2d>& edges, const std::shared_ptr<rtk::gl::texture2d>& scene)
    {
        static auto mesh = create(rtk::geometry::primitive::quad());

        static auto vis_shader = get_contour_shader();

        glClear(GL_COLOR_BUFFER_BIT);

        vis_shader->set_variable("edge", 0, *edges);
        vis_shader->set_variable("d", 1, *scene);

        mesh.draw(*vis_shader);

        return nullptr;
    }

    void
    overlay(const std::shared_ptr<rtk::gl::texture2d>& a, const std::shared_ptr<rtk::gl::texture2d>& b)
    {
        static auto mesh = create(rtk::geometry::primitive::quad());

        static auto vis_shader = get_overlay_shader();

        glClear(GL_COLOR_BUFFER_BIT);

        vis_shader->set_variable("fore", 1, *a);
        vis_shader->set_variable("back", 2, *b);

        mesh.draw(*vis_shader);
    }

    std::shared_ptr<rtk::gl::texture2d>
    draw_tex(const std::shared_ptr<rtk::gl::texture2d>& scene)
    {
        static auto mesh = create(rtk::geometry::primitive::quad());

        static auto vis_shader = get_tex_shader();

        glClear(GL_COLOR_BUFFER_BIT);

        vis_shader->set_variable("d", 1, *scene);

        mesh.draw(*vis_shader);

        return nullptr;
    }

    std::shared_ptr<rtk::gl::texture2d>
    object_pass(const rtk::camera& cam, const scene& ctx)
    {
        rtk::gl::reset_framebuffer();
        using namespace rtk::literals;

        static auto out = rtk::gl::create_texture(
                cam.get_resolution(),
                rtk::graphics::pixel_format::rgb_byte);

        static rtk::gl::framebuffer shadow_buf(*out);

        shadow_buf.activate_raw();

        static auto rboId = [&] {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out->get_id(), 0);

            GLuint rboId;
            glGenRenderbuffers(1, &rboId);
            glBindRenderbuffer(GL_RENDERBUFFER, rboId);

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                  cam.get_resolution().width, cam.get_resolution().height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER,      // 1. fbo target: GL_FRAMEBUFFER
                                      GL_DEPTH_ATTACHMENT, // 2. attachment point
                                      GL_RENDERBUFFER,     // 3. rbo target: GL_RENDERBUFFER
                                      rboId);              // 4. rbo ID

            return rboId;
        }();

        assert(shadow_buf.complete());

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        static auto shader = get_object_shader();

        shader->set_variable("vp", cam.get_vp_matrix());

        shadow_buf.set_viewport();
        int num = 1;
        for (auto& obj : ctx.objects)
        {
            if (obj.wire)
            {
                continue;
            }
            shader->set_variable("obj_id", (num++) / 255.f);
            shader->set_variable("model", obj.transform->get_world_mat4());
            obj.mesh->draw(*shader);
        }

        rtk::gl::reset_framebuffer();

        return out;
    }

    std::shared_ptr<rtk::gl::texture2d>
    geometry_pass(const rtk::camera& cam, const scene& ctx)
    {
        using namespace rtk::literals;
        static auto out = rtk::gl::create_texture(
                cam.get_resolution(),
                rtk::graphics::pixel_format::gl_depth16);
        static rtk::gl::framebuffer shadow_buf(*out);

        auto shader = get_geometry_shader();

        shader->set_variable("vp", cam.get_vp_matrix());

        shadow_buf.activate_depth();
        shadow_buf.set_viewport();

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glClear(GL_DEPTH_BUFFER_BIT);

        //cam.activate();
        for (auto& obj : ctx.objects)
        {
            shader->set_variable("model", obj.transform->get_world_mat4());
            if (obj.wire)
            {
                continue;
            }
            obj.mesh->draw(*shader);
        }

        return out;
    }

    struct shadow_ctx
    {
        std::vector<std::shared_ptr<rtk::gl::texture2d>> sms;
        std::vector<glm::mat4> light_transforms;

        shadow_ctx(int len) : sms(len), light_transforms(len) {
            using namespace rtk::literals;

            for (auto& sm : sms)
            {
                sm = rtk::gl::create_texture(
                        rtk::resolution(8192_px, 8192_px),
                        rtk::graphics::pixel_format::gl_depth16);
            }
        }
    };
    void render_one(
            const glm::vec3& cp,
            const glm::mat4& vp,
            const app::scene& ctx,
            const shadow_ctx& shadow,
            const app::renderable& obj)
    {
        auto mat = obj.mat.get();
        auto& shader = mat->go();

        shader.set_variable("camera_position", cp);
        shader.set_variable("vp", vp);
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

    void render_one(
            const rtk::camera& cam,
            const app::scene& ctx,
            const shadow_ctx& shadow,
            const app::renderable& obj)
    {
        cam.activate();
        render_one(cam.get_transform()->get_pos(), cam.get_vp_matrix(),
                ctx, shadow, obj);
    }

    std::shared_ptr<rtk::gl::texture2d>
    render_to_tex(const glm::mat4& vp, const glm::vec3& pos, rtk::resolution sz, const app::scene& ctx)
    {
        using namespace rtk::literals;

        rtk::gl::reset_framebuffer();
        static shadow_ctx shadow(ctx.lights.size());

        glCullFace(GL_FRONT);
        for (int i = 0; i < ctx.lights.size(); ++i)
        {
            auto [_, vp] = light_pass(ctx, ctx.lights[i], shadow.sms[i]);
            shadow.light_transforms[i] = vp;
        }
        glCullFace(GL_BACK);

        static auto out = rtk::gl::create_texture(
                sz,
                rtk::graphics::pixel_format::rgba_byte);

        static rtk::gl::framebuffer shadow_buf(*out);

        shadow_buf.activate_raw();

        static auto rboId = [&] {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out->get_id(), 0);

            GLuint rboId;
            glGenRenderbuffers(1, &rboId);
            glBindRenderbuffer(GL_RENDERBUFFER, rboId);

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                  sz.width, sz.height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER,      // 1. fbo target: GL_FRAMEBUFFER
                                      GL_DEPTH_ATTACHMENT, // 2. attachment point
                                      GL_RENDERBUFFER,     // 3. rbo target: GL_RENDERBUFFER
                                      rboId);              // 4. rbo ID

            return rboId;
        }();

        glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        for (auto& obj : ctx.objects)
        {
            rtk::set_viewport(rtk::screen_rect{{rtk::pixels(0), rtk::pixels(0)}, sz});
            render_one(pos, vp, ctx, shadow, obj);
        }

        rtk::gl::reset_framebuffer();

        return out;
    }

    std::shared_ptr<rtk::gl::texture2d>
            render_to_tex(const rtk::camera& cam, const app::scene& ctx)
    {
        return render_to_tex(cam.get_vp_matrix(), cam.get_transform()->get_pos(), cam.get_resolution(),
                      ctx);
    }
}
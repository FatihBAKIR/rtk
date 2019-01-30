//
// Created by fatih on 1/29/19.
//

#pragma once

#include <string>
#include <rtk/gl/mesh.hpp>
#include <rtk/physics/transform.hpp>
#include "material.hpp"
#include <rtk/camera.hpp>

namespace app
{
    struct renderable
    {
        std::string name;
        const rtk::gl::mesh* mesh;
        std::shared_ptr<rtk::transform> transform = std::make_shared<rtk::transform>();
        std::shared_ptr<app::material> mat;
        bool wire = false;
        bool cast_shadow = true;
    };

    struct scene;
    void render(const rtk::camera& cam, const app::scene& ctx);
}
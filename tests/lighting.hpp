//
// Created by fatih on 1/29/19.
//

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <rtk/gl/program.hpp>
#include <rtk/physics/transform.hpp>

namespace app
{
    struct renderable;

    struct ambient_light
    {
        glm::vec3 ambient;
    };

    struct spot_light
    {
        glm::vec3 color;
        std::shared_ptr<rtk::transform> transform = std::make_shared<rtk::transform>();
    };

    inline void apply(rtk::gl::program& p, const std::string& base, const spot_light& pl)
    {
        p.set_variable(base + ".intensity", pl.color);
        p.set_variable(base + ".position", pl.transform->get_pos());
    }

    inline void apply(rtk::gl::program& p, const ambient_light& al)
    {
        p.set_variable("ambient_light", al.ambient);
    }
}
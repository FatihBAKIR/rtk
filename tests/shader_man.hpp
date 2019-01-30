//
// Created by fatih on 1/29/19.
//

#pragma once

#include <memory>
#include <rtk/rtk_fwd.hpp>
#include <string>

namespace app
{
    std::shared_ptr<rtk::gl::program>
    load_shader(const std::string& vert, const std::string frag);
}
//
// Created by fatih on 1/29/19.
//

#pragma once

#include <rtk/gl/program.hpp>

namespace app
{
    struct material
    {
    public:
        virtual rtk::gl::program& go() = 0;
        virtual std::unique_ptr<material> clone() const = 0;
        virtual ~material() = default;
    };
}
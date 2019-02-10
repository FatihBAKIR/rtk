//
// Created by fatih on 1/29/19.
//

#pragma once

#include <vector>
#include "renderable.hpp"
#include "lighting.hpp"

namespace app
{
    struct scene
    {
        std::vector<renderable> objects;
        std::vector<area_light> lights;
        ambient_light ambient;
    };
}
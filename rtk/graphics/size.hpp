//
// Created by fatih on 19.03.2017.
//

#ifndef RENDERER_SIZE_HPP
#define RENDERER_SIZE_HPP

#include <rtk/graphics/units.hpp>

namespace rtk
{
    template <class T>
    struct size {
        T width, height;

        size(const T& w, const T& h) :
                width(w), height(h) {}
    };

    using resolution = size<rtk::pixels>;
    using physical_size = size<rtk::millimeters>;
}


#endif //RENDERER_SIZE_HPP

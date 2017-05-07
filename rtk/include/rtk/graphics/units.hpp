//
// Created by fatih on 19.03.2017.
//

#pragma once

#include <cstddef>
#include <boost/serialization/strong_typedef.hpp>

namespace rtk {

    BOOST_STRONG_TYPEDEF(std::size_t, pixels);
    BOOST_STRONG_TYPEDEF(std::size_t, millimeters);

    namespace literals
    {
        inline pixels operator""_px(unsigned long long val)
        {
            return pixels{val};
        }

        inline millimeters operator""_mm(unsigned long long val)
        {
            return millimeters{val};
        }
    }
}


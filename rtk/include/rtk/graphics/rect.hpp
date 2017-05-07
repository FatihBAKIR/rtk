//
// Created by fatih on 5/6/17.
//

#include <rtk/graphics/size.hpp>

namespace RTK_NAMESPACE
{
    template <class of_t>
    struct rect
    {
        point2d<of_t> m_position;
        size<of_t> m_size;
    };

    using screen_rect = rect<pixels>;
}

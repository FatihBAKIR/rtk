//
// Created by fatih on 5/6/17.
//

#ifndef RTK_UTILITY_HPP_HPP
#define RTK_UTILITY_HPP_HPP

#include <rtk/graphics/rect.hpp>
#include <glm/vec2.hpp>

namespace RTK_NAMESPACE
{
    void set_viewport(screen_rect rect);
    screen_rect get_rect(const resolution& res, const glm::vec2& vp_pos, const glm::vec2& vp_size);
}

#endif //RTK_UTILITY_HPP_HPP

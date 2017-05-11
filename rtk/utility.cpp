//
// Created by fatih on 5/6/17.
//

#include <rtk/utility.hpp>
#include <glad/glad.h>

namespace RTK_NAMESPACE
{
    void set_viewport(screen_rect rect)
    {
        glViewport(rect.m_position.x, rect.m_position.y, rect.m_size.width, rect.m_size.height);
    }

    screen_rect get_rect(const resolution& res, const glm::vec2& vp_pos, const glm::vec2& vp_size)
    {
        rtk::size<rtk::pixels> sz{ rtk::pixels(res.width * vp_size.x), rtk::pixels(res.height * vp_size.y) };
        rtk::point2d<rtk::pixels> pos{ rtk::pixels(res.width * vp_pos.x), rtk::pixels(res.height * vp_pos.y) };
        return {pos, sz};
    }
}


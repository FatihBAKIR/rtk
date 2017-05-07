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
}


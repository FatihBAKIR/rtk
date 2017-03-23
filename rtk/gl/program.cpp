//
// Created by fatih on 23.03.2017.
//

#include "program.hpp"
#include <rtk/gl/shader.hpp>

namespace rtk
{
namespace gl
{
    program::program() {
        id = glCreateProgram();
    }

    void program::attach(const vertex_shader & sh) {
        glAttachShader(id, sh.get_id());
    }

    void program::attach(const fragment_shader & sh) {
        glAttachShader(id, sh.get_id());
    }

    void program::link() {
        glLinkProgram(id);
        Ensures(glIsProgram(id));
    }

    void program::use() {
        glUseProgram(id);
    }
}
}


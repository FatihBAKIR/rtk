//
// Created by fatih on 23.03.2017.
//

#ifndef RENDERER_PROGRAM_HPP
#define RENDERER_PROGRAM_HPP

#include <glad/glad.h>
#include <rtk/rtk_fwd.hpp>
#include <string>

namespace rtk
{
namespace gl
{
class program {
    GLuint id;

public:
    program();

    void attach(const vertex_shader &);
    void attach(const fragment_shader &);

    void link();

    void use();
};

program load_shader(const std::string& name);
}
}


#endif //RENDERER_PROGRAM_HPP

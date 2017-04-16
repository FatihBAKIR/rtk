//
// Created by fatih on 23.03.2017.
//

#ifndef RENDERER_PROGRAM_HPP
#define RENDERER_PROGRAM_HPP

#include <glad/glad.h>
#include <rtk/rtk_fwd.hpp>
#include <string>
#include <glm/fwd.hpp>

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
    void attach(const geometry_shader &);

    void set_variable(const std::string& name, const glm::vec3&);
    void set_variable(const std::string& name, int);
    void set_variable(const std::string& name, float);
    void set_variable(const std::string& name, const glm::mat4&);

    void link();

    void use();
};
}
}


#endif //RENDERER_PROGRAM_HPP

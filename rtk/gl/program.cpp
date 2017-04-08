//
// Created by fatih on 23.03.2017.
//

#include "program.hpp"
#include <rtk/gl/shader.hpp>
#include <fstream>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
	std::string read_text_file(const std::string& path)
	{
		std::ifstream f(path);
		return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
	}
}

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

    void program::attach(const geometry_shader & sh) {
        glAttachShader(id, sh.get_id());
    }

    void program::link() {
        glLinkProgram(id);

        GLint success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);

        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            throw std::runtime_error(std::string("link failed: ") + infoLog);
        }

        Ensures(glIsProgram(id));
    }

    void program::use() {
        assert(glIsProgram(id));
        glUseProgram(id);
    }

    void program::set_variable(const std::string& name, const glm::vec3& v)
    {
        use();
        GLint lightPosLoc = glGetUniformLocation(id, name.c_str());
        glUniform3f(lightPosLoc, v.x, v.y, v.z);
    }

    void program::set_variable(const std::string& name, int v)
    {
        use();
        GLint loc = glGetUniformLocation(id, name.c_str());
        glUniform1i(loc, v);
    }

    void program::set_variable(const std::string& name, float v)
    {
        use();
        GLint loc = glGetUniformLocation(id, name.c_str());
        glUniform1f(loc, v);
    }

    void program::set_variable(const std::string& name, const glm::mat4& v)
    {
        use();
        GLint loc = glGetUniformLocation(id, name.c_str());
        glUniformMatrix4fv(loc, 1, false, glm::value_ptr(v));
    }
}
}


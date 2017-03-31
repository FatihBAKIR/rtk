//
// Created by fatih on 23.03.2017.
//

#include "program.hpp"
#include <rtk/gl/shader.hpp>
#include <fstream>

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
        glUseProgram(id);
    }
}
}


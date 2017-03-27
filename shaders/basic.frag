#version 330 core

out vec4 color;

in vec3 vert_color;

void main()
{
    color = vec4(vert_color, 1.0f);
}


#version 330 core

out vec4 color;

in VS_OUT {
    vec3 color;
    vec3 normal;
} fs_in;

void main()
{
    color = vec4((vec3(1.0) + fs_in.normal) * 0.5, 1.0f);
}


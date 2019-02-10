#version 330 core
layout (location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 vp;

void main()
{
    TexCoords = normalize(vec3(model * vec4(position, 1.0)));
    gl_Position = vp * model * vec4(position, 1.0);
}
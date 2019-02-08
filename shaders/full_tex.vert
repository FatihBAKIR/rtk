#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 uv;

out vec2 pix_uv;

void main(){
    pix_uv = uv;
    gl_Position = vec4(position, 1.0);
}
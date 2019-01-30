#version 330 core

in vec2 pix_uv;

uniform sampler2D d;
uniform sampler2D edge;

out vec4 final_color;

void main() {
    vec4 d_val = texture(d, pix_uv);
    float e = texture(edge, pix_uv).r;

	final_color = d_val * (1 - e);
}

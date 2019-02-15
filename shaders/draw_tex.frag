#version 330 core

in vec2 pix_uv;

uniform sampler2D d;

out vec4 final_color;

void main() {
    vec4 d_val = texture(d, pix_uv);

	final_color = d_val;
}

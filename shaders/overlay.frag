#version 330 core

in vec2 pix_uv;

uniform sampler2D back;
uniform sampler2D fore;

out vec4 final_color;

void main() {
    vec4 back_val = texture(back, pix_uv);
    vec4 fore_val = texture(fore, pix_uv);

	final_color = fore_val.a > 0 ? fore_val : back_val;
}

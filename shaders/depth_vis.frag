#version 330 core

in vec2 pix_uv;

uniform sampler2D d;
out vec4 final_color;

void main() {
    float d_val = texture(d, pix_uv).r;
	final_color = vec4(d_val, d_val, d_val, 1.0);
}

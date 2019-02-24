#version 330 core

in vec2 pix_uv;

uniform sampler2D d;
//uniform bool flip_y;

out vec4 final_color;

void main() {
    vec2 uv = pix_uv;
    //if (flip_y)
    //{
    //    uv.y = 1 - uv.y;
    //}

    vec4 d_val = texture(d, pix_uv);

	final_color = d_val;
}

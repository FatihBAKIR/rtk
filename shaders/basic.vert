#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
uniform mat4 vp;
uniform mat4 model;

out VS_OUT {
    vec3 color;
    vec3 normal;
} vs_out;

vec3 do_lighting(vec3 pos)
{
    vec3 light_pos = vec3(0, 10, 0);
    vec3 light_color = vec3(1, 1, 1);

    vec3 light_dir = normalize(light_pos - pos);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diff_col = diff * light_color;

    return diff_col;
}

void main()
{
    vec4 pos = vp * model * vec4(position, 1.0);
    gl_Position = pos;
    vs_out.color = do_lighting(vec3(pos));
    vs_out.normal = normal;
}

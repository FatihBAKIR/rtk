#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec4 light_pos[8];
out vec4 world_position;
out vec4 world_normal;

struct PointLight
{
    vec3 intensity;
    vec3 position;
    sampler2D shadowTex;
    mat4 transform;
};

uniform mat4 model;
uniform mat4 vp;

uniform PointLight point_light[8];
uniform int number_of_point_lights;

void main()
{
    world_position = model * vec4(position, 1.0);
    for (int i = 0; i < number_of_point_lights; ++i)
    {
        light_pos[i] = point_light[i].transform * world_position;
    }
    world_normal = vec4(normalize(vec3(transpose(inverse(model)) * vec4(normal, 0.0f))), 0);
	gl_Position = vp * world_position;
}
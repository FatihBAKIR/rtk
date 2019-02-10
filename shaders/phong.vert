#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 b_uv;
layout (location = 3) in vec3 bitan;
layout (location = 4) in vec3 tan;

struct PhongMaterial
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float phong_exponent;
    bool textured;
    bool normaled;
};

uniform float uv_scale;

out vec4 light_pos[8];
out vec4 world_position;
out vec3 world_normal;
out vec2 uv;
out mat3 tbn;

struct PointLight
{
    vec3 intensity;
    vec3 position;
    float size;
    sampler2D shadowTex;
    mat4 transform;
};

uniform PhongMaterial material;

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

    world_normal = normalize(vec3(transpose(inverse(model)) * vec4(normal, 0.0f)));

    if (material.normaled)
    {
        vec3 T = normalize(vec3(model * vec4(tan,   0.0)));
        vec3 B = normalize(vec3(model * vec4(bitan, 0.0)));
        vec3 N = normalize(vec3(model * vec4(normal,    0.0)));
        tbn = mat3(T, B, N);
    }

	gl_Position = vp * world_position;
	uv = b_uv * uv_scale;
}
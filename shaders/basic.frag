#version 330 core
#undef HAS_UV

in vec4 light_pos;
in vec4 world_position;
in vec4 world_normal;
in vec2 frag_uv;

out vec4 final_color;

struct PointLight
{
    vec3 intensity;
    vec3 position;
};

struct DirectionalLight
{
    vec3 intensity;
    vec3 direction;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float phong_exponent;
};

uniform Material material;
uniform vec3 ambient_light;
uniform PointLight point_light[8];
uniform int number_of_point_lights;
uniform DirectionalLight directional_light;
uniform vec3 camera_position;
uniform sampler2D tex;
uniform sampler2D shadowTex;

vec3 get_diffuse()
{
    return material.diffuse;
}

float visible(PointLight l)
{
    float bias = max(0.05 * (1.0 - dot(vec3(world_normal), normalize(l.position - vec3(world_position)))), 0.005);
    vec3 projCoords = light_pos.xyz / light_pos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowTex, projCoords.xy).r;
    float currentDepth = projCoords.z;
    return currentDepth - bias/2 > closestDepth ? 0.0 : 1.0;
}

vec3 computeRadiancePointLight(PointLight pointlight, float distance);
vec3 computeRadianceDirectionalLight(DirectionalLight directional_light);
vec3 computeDiffuseReflectance(Material material, vec3 to_light, vec3 normal);
vec3 computeReflectance(Material material, vec3 to_light, vec3 normal, vec3 to_eye);

void main()
{
    vec3 color = material.ambient * ambient_light;

    vec3 to_eye = normalize(camera_position - vec3(world_position));

    for (int i = 0; i < number_of_point_lights; ++i)
    {
        vec3 to_light = point_light[i].position - vec3(world_position);
        float distance = length(to_light);
        to_light /= distance;
        color += computeReflectance(material, to_light, vec3(world_normal), to_eye) * computeRadiancePointLight(point_light[i], distance) * visible(point_light[i]);
    }

    vec3 to_light = -directional_light.direction;
    color += computeReflectance(material, to_light, vec3(world_normal), to_eye) * computeRadianceDirectionalLight(directional_light);

	final_color = vec4(color, 1.0f);
}

vec3 computeRadiancePointLight(PointLight point_light, float distance)
{
    return point_light.intensity / (distance * distance);
}

vec3 computeRadianceDirectionalLight(DirectionalLight directional_light)
{
    return directional_light.intensity;
}

vec3 computeReflectance(Material material, vec3 to_light, vec3 normal, vec3 to_eye)
{
    vec3 diffuse_reflectance = max(dot(to_light, normal), 0.0f) * get_diffuse();
    vec3 specular_reflectance = pow(max(dot(normalize(to_light + to_eye), normal), 0.0f), material.phong_exponent) * material.specular;

    return diffuse_reflectance + specular_reflectance;
}
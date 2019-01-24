#version 330 core
#undef HAS_UV

in vec4 light_pos[8];
in vec4 world_position;
in vec4 world_normal;

out vec4 final_color;

struct PointLight
{
    vec3 intensity;
    vec3 position;
    sampler2D shadowTex;
    mat4 transform;
};

struct DirectionalLight
{
    vec3 intensity;
    vec3 direction;
};

struct PhongMaterial
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float phong_exponent;
};

uniform PhongMaterial material;

uniform vec3 ambient_light;

uniform PointLight point_light[8];
uniform int number_of_point_lights;

uniform DirectionalLight directional_light;
uniform vec3 camera_position;

vec3 get_diffuse()
{
    return material.diffuse;
}

float visible(int i)
{
    vec3 projCoords = light_pos[i].xyz / light_pos[i].w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
    {
        return 1.0;
    }

    float closestDepth = texture(point_light[i].shadowTex, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(vec3(world_normal), normalize(point_light[i].position - vec3(world_position)))), 0.005);

    return currentDepth - bias/2 > closestDepth ? 0.0 : 1.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(point_light[i].shadowTex, 0);
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(point_light[i].shadowTex, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 25.0;
    return shadow;
}

vec3 computeRadiancePointLight(PointLight pointlight, float distance);
vec3 computeRadianceDirectionalLight(DirectionalLight directional_light);
vec3 computeDiffuseReflectance(PhongMaterial material, vec3 to_light, vec3 normal);
vec3 computeReflectance(PhongMaterial material, vec3 to_light, vec3 normal, vec3 to_eye);

void main()
{
    vec3 color = material.ambient * ambient_light;

    vec3 to_eye = normalize(camera_position - vec3(world_position));

    for (int i = 0; i < number_of_point_lights; ++i)
    {
        vec3 to_light = point_light[i].position - vec3(world_position);
        float distance = length(to_light);
        to_light /= distance;
        color +=
            computeReflectance(material, to_light, vec3(world_normal), to_eye) *
            computeRadiancePointLight(point_light[i], distance) *
            visible(i);
    }

    //vec3 to_light = -directional_light.direction;
    //color += computeReflectance(material, to_light, vec3(world_normal), to_eye) * computeRadianceDirectionalLight(directional_light);

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

vec3 computeReflectance(PhongMaterial material, vec3 to_light, vec3 normal, vec3 to_eye)
{
    vec3 diffuse_reflectance = max(dot(to_light, normal), 0.0f) * get_diffuse();
    vec3 specular_reflectance = pow(max(dot(normalize(to_light + to_eye), normal), 0.0f), material.phong_exponent) * material.specular;

    return diffuse_reflectance + specular_reflectance;
}
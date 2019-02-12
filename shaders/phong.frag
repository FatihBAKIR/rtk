#version 330 core
#undef HAS_UV

in vec4 light_pos[8];
in vec4 world_position;
in vec3 world_normal;
in vec2 uv;
in mat3 tbn;

out vec4 final_color;

struct PointLight
{
    vec3 intensity;
    vec3 position;
    float size;
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
    bool textured;
    bool normaled;
    bool specd;
};

uniform PhongMaterial material;

uniform vec3 ambient_light;

uniform PointLight point_light[8];
uniform int number_of_point_lights;

uniform DirectionalLight directional_light;
uniform vec3 camera_position;

uniform sampler2D tex;
uniform sampler2D spec;
uniform sampler2D normal_map;

vec3 get_ambient()
{
    if (material.textured)
    {
        return texture(tex, uv).rgb * material.ambient;
    }
    return material.ambient;
}

vec3 get_diffuse()
{
    if (material.textured)
    {
        return texture(tex, uv).rgb * material.diffuse;
    }
    return material.diffuse;
}

vec3 get_specular()
{
    if (material.specd)
    {
        return texture(spec, uv).rgb * material.specular;
    }
    return material.specular;
}

vec3 get_normal()
{
    if (!material.normaled) return world_normal;
    vec3 normal = texture(normal_map, uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    return normalize(tbn * normal);
}

float visible(int i)
{
    vec3 projCoords = light_pos[i].xyz / light_pos[i].w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
    {
        return 1.0;
    }

    float fragment_depth = projCoords.z;

    float distance = length(point_light[i].position - vec3(world_position));

    float bias = 0.005f;//max(0.05 * (1.0 - dot(get_normal(), normalize(frag_to_light))), 0.005);

    float total_depth = 0;
    int total = 0;
    vec2 texelSize = 1.0 / textureSize(point_light[i].shadowTex, 0);

    int blocker_sz = int (log2(point_light[i].size) * log2(point_light[i].size) * 0.5);

    for(int x = -blocker_sz; x <= blocker_sz; ++x)
    {
        for(int y = -blocker_sz; y <= blocker_sz; ++y)
        {
            float pcfDepth = texture(point_light[i].shadowTex, projCoords.xy + vec2(x, y) * texelSize).r;
            if (fragment_depth - bias > pcfDepth)
            {
                total_depth += pcfDepth;
                ++total;
            }
        }
    }

    if (total == 0) return 1.0;

    float avg_d = total_depth / total;
    float w_pen = ((fragment_depth - avg_d) / avg_d);

    int sz = min(10, max(1, int(ceil(15 * point_light[i].size * w_pen))));

    float shadow = 0.0;
    for(int x = -sz; x <= sz; ++x)
    {
        for(int y = -sz; y <= sz; ++y)
        {
            float pcfDepth = texture(point_light[i].shadowTex, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += fragment_depth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= pow(2 * sz + 1, 2);

    return 1 - shadow;
}

vec3 computeRadiancePointLight(PointLight pointlight, float distance);
vec3 computeRadianceDirectionalLight(DirectionalLight directional_light);
vec3 computeDiffuseReflectance(PhongMaterial material, vec3 to_light, vec3 normal);
vec3 computeReflectance(PhongMaterial material, vec3 to_light, vec3 normal, vec3 to_eye);

void main()
{
    /*if (material.normaled)
    {
        final_color = vec4((get_normal() + 1.0) / 2.0, 1.0);
        return;
    }*/
    vec3 color = get_ambient() * ambient_light;

    vec3 to_eye = normalize(camera_position - vec3(world_position));

    for (int i = 0; i < number_of_point_lights; ++i)
    {
        vec3 to_light = point_light[i].position - vec3(world_position);
        float distance = length(to_light);
        to_light /= distance;
        color +=
            computeReflectance(material, to_light, get_normal(), to_eye) *
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
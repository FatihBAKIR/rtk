#version 330 core

in vec3 world;

uniform sampler2D front;
uniform sampler2D back;
uniform float overlap;

vec2 theta_phi(vec3 point)
{
    // point must have length == 1

    float theta = acos(point.y);
    float phi = atan(-point.z, point.x);

    return vec2(theta, phi);
}

const float PI = 3.1415926535897932384626433832795;

vec4 sampleSphere(vec3 normal)
{
    if (dot(normal, vec3(0, -1, 0)) > 0.9)
    {
        return vec4(0);
    }

    vec2 coord = theta_phi(normal);

    float front_w = 0;
    float back_w = 0;

    /*float interest = abs(coord.y / PI);
    if (interest > 1 - overlap || interest < overlap)
    {
        return vec4(1, 0, 0, 1);
    }*/

    if (coord.y > 0)
    {
        front_w = 0;
        back_w = 1;
    }
    else
    {
        front_w = 1;
        back_w = 0;
    }

    normal *= 1 - overlap;
    normal += vec3(1, 1, 1);
    normal /= 2;

    vec4 res = vec4(0);

    res += texture(back, vec2(-normal.x, -normal.y), 1) * back_w;
    res += texture(front, vec2(normal.x, -normal.y), 1) * front_w;

    return res;
}

void main() {
    gl_FragColor = sampleSphere(normalize(world));
}

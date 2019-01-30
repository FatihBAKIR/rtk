#version 330 core

in vec2 pix_uv;

uniform sampler2D d;
out float color;

mat3 sx = mat3(
    1.0, 2.0, 1.0,
    0.0, 0.0, 0.0,
   -1.0, -2.0, -1.0
);

mat3 sy = mat3(
    1.0, 0.0, -1.0,
    2.0, 0.0, -2.0,
    1.0, 0.0, -1.0
);

void main() {
    mat3 I;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float s = texelFetch(d, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0).r * 50.f;
            I[i][j] = s;
        }
    }

    float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]);
    float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

    float g = sqrt(pow(gx, 2.0) + pow(gy, 2.0));
    color = g;
}

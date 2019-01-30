#version 330 core

uniform sampler2D d;
out float color;

void main() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float s = texelFetch(d, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0).r;
            if (s > 0.5)
            {
                color = s;
                return;
            }
        }
    }
    color = 0;
}

#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform sampler2D tx;

void main()
{
//    FragColor = texture(skybox, TexCoords);
    FragColor = texture(tx, TexCoords.xy);
}
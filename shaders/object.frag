#version 330 core

uniform float obj_id;

// Ouput data
out float frag_color;

void main()
{
    frag_color = obj_id;
}
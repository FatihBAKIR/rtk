#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 32) out;

vec3 createPerp(vec3 p1, vec3 p2)
{
  vec3 invec = normalize(p2 - p1);
  vec3 ret = cross( invec, vec3(0.0, 0.0, 1.0) );
  if ( length(ret) == 0.0 )
  {
    ret = cross( invec, vec3(0.0, 1.0, 0.0) );
  }
  return ret;
}

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void enlarge(vec3 p1, vec3 p2)
{
   vec3 axis = p2 - p1;

   vec3 perpx = createPerp(p1, p2);
   int segs = 16;

   mat4 rotation = rotationMatrix(axis, 2 * 3.14159 / segs);
   vec3 pos = perpx * 0.005 * 1;
   for(int i=0; i<segs; i++) {
      gl_Position = projection * view * model * vec4(p1 + pos, 1.0); EmitVertex();
      gl_Position = projection * view * model * vec4(p2 + pos, 1.0); EmitVertex();
      pos = vec3(rotation * vec4(pos, 0.0));
   }

   EndPrimitive();
}

void main() {
    enlarge(gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w, gl_in[1].gl_Position.xyz / gl_in[0].gl_Position.w);
}

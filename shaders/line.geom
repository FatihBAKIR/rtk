#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 32) out;

in VS_OUT {
    vec3 color;
    vec3 normal;
} gs_in[];

out VS_OUT
{
    vec3 color;
    vec3 normal;
} gs_out;

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
   gs_out.color = vec3(0.2, 0.5, 1);
   gs_out.normal = gs_in[0].normal;
   vec3 axis = p2 - p1;

   vec3 perpx = createPerp(p1, p2);
   int segs = 8;

   mat4 rotation = rotationMatrix(axis, 2 * 3.14159 / segs);
   vec3 pos = perpx * 0.00375;
   for(int i=0; i<segs; i++) {
      gl_Position = vec4(p1 + pos, 1.0); EmitVertex();
      gl_Position = vec4(p2 + pos, 1.0); EmitVertex();
      pos = vec3(vec4(pos, 0.0) * rotation);
   }

   EndPrimitive();
}

void main() {
    enlarge(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz);

}

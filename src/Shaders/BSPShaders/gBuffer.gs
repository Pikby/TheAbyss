#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


out GS_OUT
{
  vec2 TexCoord;
  vec2 TexCells;
  vec2 TexOrigin;
  vec3 FinNormal;
  vec3 FragPos;
  float AO;
}
gs_out;

in VS_OUT
{
  vec2 TexCoord;
  vec2 TexCells;
  vec2 TexOrigin;
  vec3 FinNormal;
  vec3 FragPos;
  float AO;
} vs_out[];


void passData(int index)
{
  gs_out.TexCoord = vs_out[index].TexCoord;
  gs_out.TexCells = vs_out[index].TexCells;
  gs_out.TexOrigin = vs_out[index].TexOrigin;
  //gs_out.FinNormal = vs_out[index].FinNormal;
  gs_out.FragPos = vs_out[index].FragPos;
  gs_out.AO = vs_out[index].AO;
}

void main()
{

  vec3 p1 = vs_out[0].FragPos;
  vec3 p2 = vs_out[1].FragPos;
  vec3 p3 = vs_out[2].FragPos;
  vec3 norm = normalize(cross(p1-p2,p1-p3));
  gs_out.FinNormal = norm;

  gl_Position = gl_in[0].gl_Position;
  passData(0);
  EmitVertex();

  gl_Position = gl_in[1].gl_Position;
  passData(1);
  EmitVertex();

  gl_Position = gl_in[2].gl_Position;
  passData(2);
  EmitVertex();
  EndPrimitive();
}

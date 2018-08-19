#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec2 TexCoord;
out vec2 TexCells;
out vec2 TexOrigin;
out vec3 FinNormal;
out vec3 FragPos;
out float AO;

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
  TexCoord = vs_out[index].TexCoord;
  TexCells = vs_out[index].TexCells;
  TexOrigin = vs_out[index].TexOrigin;
  FinNormal = vs_out[index].FinNormal;
  FragPos = vs_out[index].FragPos;
  AO = vs_out[index].AO;
}

void main()
{
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

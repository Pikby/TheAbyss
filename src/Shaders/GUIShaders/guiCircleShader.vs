#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
out vec2 FragPos;

uniform mat3 model;
uniform mat4 projection;
void main()
{

  vec3 pos = model*vec3(vertex.x,vertex.y,1);
  FragPos = vec2(pos.x,pos.y);
  gl_Position = projection*vec4(pos.x,pos.y, 0.0, 1.0);
  TexCoords = vertex.zw;
}

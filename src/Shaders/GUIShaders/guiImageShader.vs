#version 330 core
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texCoords;
out vec2 TexCoords;
uniform mat3 model;
uniform mat4 projection;
void main()
{
  vec3 pos = model*vec3(vertex.x,vertex.y,1);
  gl_Position = projection*vec4(pos.x,pos.y, 0.0, 1.0);
  TexCoords = texCoords;
}

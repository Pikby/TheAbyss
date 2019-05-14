#version 330 core
layout (location = 0) in vec2 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec2 texCoords;
out vec2 vs_texCoords;
uniform mat3 model;
uniform mat4 projection;

uniform float depth;

void main()
{
  vec3 pos = model*vec3(vertex.x,vertex.y,1);
  gl_Position = projection*vec4(pos.x,pos.y, depth, 1.0);
  vs_texCoords = texCoords;
}

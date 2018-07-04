#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normVec;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 FinNormal;
out vec3 FragPos;
out float ClipSpaceDepth;

uniform mat4 view;
uniform mat4 projection;

void main()
{
  TexCoord = texCoord;
  FinNormal = normVec;
  FragPos = position;
  gl_Position = projection*view*vec4(position, 1.0f);
  ClipSpaceDepth = gl_Position.z;
}

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normVec;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 NormVec;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

void main()
{
  TexCoord = texCoord;
  NormVec = normVec;
  FragPos = position;
  FragPosLightSpace = lightSpace*vec4(position,1.0f);
  gl_Position = projection*view*vec4(position, 1.0f);
}

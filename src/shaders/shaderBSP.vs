#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normVec;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 NormVec;
out vec3 FragPos;
out vec4 FragPosLightSpace;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
      gl_Position = projection*view*model*vec4(position, 1.0f);
      TexCoord = texCoord;
      NormVec = mat3(transpose(inverse(model)))*normVec;
      FragPos = vec3(model*vec4(position,1.0f));
      FragPosLightSpace = vec4(FragPos,1.0f);
}

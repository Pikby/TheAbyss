#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 modelMat;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

out VS_OUT
{
  vec3 FinNormal;
  vec3 FragPos;
} vs_out;
void main()
{
    gl_Position = projection*view*modelMat*vec4(position, 1.0f);
    vs_out.FragPos = vec3(modelMat* vec4(position,1.0));
    vs_out.FinNormal = normalize(mat3(transpose(inverse(modelMat))) * normal);
}

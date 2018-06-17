#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

out vec3 FinNormal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main()
{
    gl_Position = projection*view*model*vec4(position, 1.0f);
    FragPos = vec3(model* vec4(position,1.0));
    FinNormal = normalize(mat3(transpose(inverse(model))) * normal);
    FragPosLightSpace = lightSpace*vec4(FragPos,1.0f);
}

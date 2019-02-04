#version 330 core
layout (location = 0) in float positionPackage;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
  int xPos = floatBitsToInt(positionPackage) & 0x3f;
  int yPos = (floatBitsToInt(positionPackage) >> 6) & 0x3f;
  int zPos = (floatBitsToInt(positionPackage) >> 12)& 0x3f;
  vec3 pos = vec3(xPos,yPos,zPos);

  gl_Position = lightSpaceMatrix * model * vec4(pos, 1.0);
}

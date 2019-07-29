#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;


void main()
{
  gPosition = vec3(0,0,0);
  gNormal = vec3(0,0,1);
  gColorSpec = vec4(1);
}

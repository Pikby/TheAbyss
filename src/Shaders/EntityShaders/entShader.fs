#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in VS_OUT
{
  vec3 FinNormal;
  vec3 FragPos;
} vs_out;

uniform vec3 objectColor;


void main()
{
  vec3 objColor = objectColor;
  vec3 finColor = objColor;



  gPosition = vs_out.FragPos;
  gNormal = vs_out.FinNormal;
  gColorSpec = vec4(finColor,1);

}

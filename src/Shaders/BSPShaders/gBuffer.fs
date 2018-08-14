#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in vec2 TexCoord;
in vec3 FinNormal;
in vec3 FragPos;
in vec2 TexCells;
in vec2 TexOrigin;

uniform sampler2D curTexture;
uniform vec3 objectColor;


vec3 getTexture()
{
  vec2 temp;
  temp.x = mod(TexCoord.x*TexCells.x,(1.0f/3.0f))+TexOrigin.x;
  temp.y = mod(TexCoord.y*TexCells.y,1)+TexOrigin.y;
  return texture(curTexture,temp).rgb;
}

void main()
{
  vec3 objColor = objectColor;
  vec3 finColor = objColor*getTexture();
  gPosition = FragPos;
  gNormal = FinNormal;
  gColorSpec = vec4(finColor,1);
}

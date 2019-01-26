#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;


in VS_OUT
{
  vec2 TexCoord;
  vec2 TexCells;
  vec2 TexOrigin;
  vec3 FinNormal;
  vec3 FragPos;
  float AO;
} vs_out;

uniform sampler2D curTexture;
uniform vec3 objectColor;


uniform int textureAtlasWidthInCells;
uniform int textureAtlasHeightInCells;





vec4 getTexture()
{
  vec2 temp;
  temp.x = mod(vs_out.TexCoord.x*vs_out.TexCells.x,1.0f/textureAtlasWidthInCells)+vs_out.TexOrigin.x;
  temp.y = mod(vs_out.TexCoord.y*vs_out.TexCells.y,1.0f/textureAtlasHeightInCells)+vs_out.TexOrigin.y;
  return texture(curTexture,temp).rgba;
}

void main()
{
  vec4 text = getTexture();
  vec3 objColor = objectColor;
  vec3 finColor = objColor*text.rgb;



  gPosition = vs_out.FragPos;
  gNormal = vs_out.FinNormal;
  gColorSpec = vec4(finColor,vs_out.AO);

}

#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;


in GS_OUT
{
  vec2 TexCoord;
  vec2 TexCells;
  vec2 TexOrigin;
  vec3 FinNormal;
  vec3 FragPos;
  float AO;
} gs_out;

uniform sampler2D curTexture;
uniform vec3 objectColor;


uniform int textureAtlasWidthInCells;
uniform int textureAtlasHeightInCells;





vec4 getTexture()
{
  vec2 temp;
  temp.x = mod(gs_out.TexCoord.x*gs_out.TexCells.x,1.0f/textureAtlasWidthInCells)+gs_out.TexOrigin.x;
  temp.y = mod(gs_out.TexCoord.y*gs_out.TexCells.y,1.0f/textureAtlasHeightInCells)+gs_out.TexOrigin.y;
  return texture(curTexture,temp).rgba;
}

void main()
{
  vec4 text = getTexture();
  vec3 objColor = objectColor;
  vec3 finColor = objColor*text.rgb;



  gPosition = gs_out.FragPos;
  gNormal = gs_out.FinNormal;
  gColorSpec = vec4(finColor,gs_out.AO);

}

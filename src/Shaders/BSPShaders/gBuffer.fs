#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;


in GS_OUT
{
  vec3 TexWeights;
  vec3 TriPos;
  vec2 TexCoord;
  vec2 TexCells;
  vec2 TexOrigins[3];
  vec3 FinNormal;
  vec3 FragPos;
  float AO;
} gs_out;

uniform sampler2D curTexture;
uniform vec3 objectColor;


uniform int textureAtlasWidthInCells;
uniform int textureAtlasHeightInCells;



vec3 getWeights()
{
  vec3 absNorm = abs(gs_out.FinNormal);
  return absNorm/(absNorm.x+absNorm.y+absNorm.z);
}


vec2 getTextureCoordinates(vec2 texPos,int id)
{
  vec2 temp;
  temp.x = mod(texPos.x,1.0f/textureAtlasWidthInCells)+gs_out.TexOrigins[id].x;
  temp.y = mod(texPos.y,1.0f/textureAtlasHeightInCells)+gs_out.TexOrigins[id].y;

  return temp;
}

vec3 getTextureAlbedo(int id)
{
  vec2 t1 = getTextureCoordinates(gs_out.TriPos.yz,id);
  vec2 t2 = getTextureCoordinates(gs_out.TriPos.xz,id);
  vec2 t3 = getTextureCoordinates(gs_out.TriPos.xy,id);

  vec3 weights = getWeights();
  vec3 albedo = weights.x*texture(curTexture,t1).rgb
              + weights.y*texture(curTexture,t2).rgb
              + weights.z*texture(curTexture,t3).rgb;

  return albedo/3.0f;

}

void main()
{



  vec3 objColor = objectColor;
  vec3 finColor = objColor*(gs_out.TexWeights.x*getTextureAlbedo(0)
                + gs_out.TexWeights.y*getTextureAlbedo(1)
                + gs_out.TexWeights.z*getTextureAlbedo(2));


  //finColor = getTextureAlbedo(1);
  gPosition = gs_out.FragPos;
  gNormal = gs_out.FinNormal;
  gColorSpec = vec4(finColor,gs_out.AO);

}

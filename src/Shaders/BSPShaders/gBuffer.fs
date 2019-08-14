#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

uniform sampler2D textureAtlas;
uniform vec3 objectColor;

uniform int textureAtlasWidthInCells;
uniform int textureAtlasHeightInCells;
float cellWidth = 1.0f/textureAtlasWidthInCells;
float cellHeight = 1.0f/textureAtlasHeightInCells;

#include "gBuffer.h"

in VS_OUT vs_out;


vec3 getWeights()
{
  vec3 absNorm = abs(normalize(vs_out.FlatNormal));
  return absNorm/(absNorm.x+absNorm.y+absNorm.z);
}


vec2 getTextureCoordinates(vec2 texPos,int id)
{
  vec2 temp;
  temp.x = mod(texPos.x,cellWidth)+vs_out.TexOrigins[id].x;
  temp.y = mod(texPos.y,cellHeight)+vs_out.TexOrigins[id].y;

  return temp;
}

vec3 getTextureAlbedo(int id)
{
  vec2 t1 = getTextureCoordinates(vs_out.LocalPos.yz,id);
  vec2 t2 = getTextureCoordinates(vs_out.LocalPos.xz,id);
  vec2 t3 = getTextureCoordinates(vs_out.LocalPos.xy,id);
  vec3 weights = getWeights();
  vec3 albedo = weights.x*texture(textureAtlas,t1).rgb
              + weights.y*texture(textureAtlas,t2).rgb
              + weights.z*texture(textureAtlas,t3).rgb;
  return albedo/3.0f;

}

void main()
{
  vec3 albedo = objectColor*(vs_out.TexWeights.x*getTextureAlbedo(0)
                           + vs_out.TexWeights.y*getTextureAlbedo(1)
                           + vs_out.TexWeights.z*getTextureAlbedo(2));

  gPosition = vs_out.FragPos;
  gNormal = vs_out.Normal;
  gColorSpec = vec4(albedo,vs_out.alpha);

}

#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in float texIds;
layout (location = 3) in float packagef;

#include "gBuffer.h"


out VS_OUT vs_out;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform int textureAtlasWidthInCells;
uniform int textureAtlasHeightInCells;
float cellWidth = 1.0f/textureAtlasWidthInCells;
float cellHeight = 1.0f/textureAtlasHeightInCells;

vec2 getOrigin(int id)
{
  vec2 ret;
  ret.x = (id % textureAtlasWidthInCells)*cellWidth;
  ret.y = (id/textureAtlasHeightInCells)*cellHeight;
  return ret;
}


void main()
{
  gl_Position = projection*view*model*vec4(position, 1.0f);
  vs_out.Normal = norm;
  vs_out.LocalPos = position;


  int iTexIds = floatBitsToInt(texIds);
  int tex0 = iTexIds & 0xff;
  int tex1 = ((iTexIds >> 8) & 0xff);
  int tex2 = ((iTexIds >> 16) & 0xff);

  vs_out.TexOrigins[0] = getOrigin(tex0);
  vs_out.TexOrigins[1] = getOrigin(tex1);
  vs_out.TexOrigins[2] = getOrigin(tex2);


  const vec3 weightTable[3]=
  {
    vec3(1,0,0),
    vec3(0,1,0),
    vec3(0,0,1)
  };
  vs_out.TexWeights = weightTable[gl_VertexID % 3];

}

#version 330 core
layout (location = 0) in float positionPackage;
layout (location = 1) in float packagef;

out VS_OUT
{
  vec2 TexCoord;
  vec2 TexCells;
  vec2 TexOrigin;
  vec3 FinNormal;
  vec3 FragPos;
  float AO;
} vs_out;




uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform int textureAtlasWidthInCells;
uniform int textureAtlasHeightInCells;

void main()
{

  int xPos = floatBitsToInt(positionPackage) & 0x3FF;
  int yPos = (floatBitsToInt(positionPackage) >> 10) & 0x3FF;
  int zPos = (floatBitsToInt(positionPackage) >> 20) & 0x3FF;

  vec3 position = vec3(xPos,yPos,zPos);
  position = floor(position/16.0f);
  int package = floatBitsToInt(packagef);
  int norm = ((package >> 24) & 0xF);
  int ao = ((package >> 24) & 0x30);
  int texCoord = ((package >> 24) & 0xC0);
  int texId = (package >> 16) & 0xFF;
  int yblocks = (package >> 8) & 0xFF;
  int xblocks = package & 0xFF;

  float cellWidth = 1.0f/textureAtlasWidthInCells;
  float cellHeight = 1.0f/textureAtlasHeightInCells;
  vec3 normVec = vec3(0.0f,0.0f,0.0f);

  vs_out.TexCells.x = xblocks;
  vs_out.TexCells.y = yblocks;

  ao = (ao >> 4) ;
  texCoord = (texCoord >> 6);

  const float AOArr[4] = float[4](1.0f,0.8f,0.5f,0.3f);
  /*
  switch(ao)
  {
    case(0): vs_out.AO = 1.0; break;
    case(1): vs_out.AO = 0.8; break;
    case(2): vs_out.AO = 0.5; break;
    case(3): vs_out.AO = 0.3; break;
  }
  */
  //vs_out.AO = 1.0f - ao*0.22f;
  vs_out.AO = AOArr[ao];



  //An attempt and removing all brances from the shaders, its essentially the same as the switch statement
  //But should run faster

  vec3 normBits;
  normBits.z = norm & 0x1;
  normBits.y = (norm >> 1) & 0x1;
  normBits.x = (norm >> 2) & 0x1;
  float modNorm = (norm >> 3) & 0x1;
  normVec = normBits -modNorm*normBits*2.0f;


  vec2 origin;
  origin.x = (texId % textureAtlasWidthInCells)*cellWidth;
  origin.y = (texId/textureAtlasHeightInCells)*cellHeight;

  vs_out.TexOrigin = origin;
  //origin = vec2(0.0f,0.0f);
  //More one liners that used to be branches, kills readibility but it works
  float texCoordx = texCoord & 0x1;
  float texCoordy = (texCoord >> 1) & 0x1;
  vs_out.TexCoord.y = origin.y + texCoordx*cellHeight;
  vs_out.TexCoord.x = origin.x + texCoordy*cellWidth;


  vs_out.FinNormal = normVec;
  vs_out.FragPos = vec3(model*vec4(position,1.0f));




  gl_Position = projection*view*model*vec4(position, 1.0f);
}

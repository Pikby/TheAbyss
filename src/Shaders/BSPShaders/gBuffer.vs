#version 330 core
layout (location = 0) in vec3 position;
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

uniform int textureAtlasWidth;
uniform int textureAtlasHeight;
uniform int cellWidth;


void main()
{

  int package = floatBitsToInt(packagef);
  int norm = ((package >> 24) & 0x7);
  int ao = ((package >> 24) & 0x38);
  int texCoord = ((package >> 24) & 0xC0);
  int texId = (package >> 16) & 0xFF;
  int yblocks = (package >> 8) & 0xFF;
  int xblocks = package & 0xFF;

  int widthOfAtlasInCells = textureAtlasWidth/cellWidth;
  int heightOfAtlasInCells = textureAtlasHeight/cellWidth;
  float cellWidth = 1.0f/widthOfAtlasInCells;
  float cellHeight = 1.0f/heightOfAtlasInCells;
  vec3 normVec = vec3(0.0f,1.0f,0.0f);

  vs_out.TexCells.x = xblocks;
  vs_out.TexCells.y = yblocks;

  ao = (ao >> 3);
  switch(ao)
  {
    case(0): vs_out.AO = 1.0; break;
    case(1): vs_out.AO = 0.8; break;
    case(2): vs_out.AO = 0.5; break;
    case(3): vs_out.AO = 0.3; break;
  }


  switch(norm)
  {
    //Front
    case(0):
      normVec = vec3(0.0f,0.0f,1.0f);
      break;
    //Back
    case(1):
      normVec = vec3(0.0f,0.0f,-1.0f);
      break;
    //Top
    case(2):
      normVec = vec3(0.0f,1.0f,0.0f);
      break;
    //Bottom
    case(3):
      normVec = vec3(0.0f,-1.0f,0.0f);
      break;
    //Right
    case(4):
      normVec = vec3(1.0f,0.0f,0.0f);
      break;
    //Left
    case(5):
      normVec = vec3(-1.0f,0.0f,0.0f);
      break;
  }


  vec2 origin;
  origin.x = (texId % widthOfAtlasInCells)*cellWidth;
  origin.y = (texId/widthOfAtlasInCells)*cellHeight;

  vs_out.TexOrigin = origin;
  //origin = vec2(0.0f,0.0f);
  if((texCoord & (1<<6)) != 0)
  {
    vs_out.TexCoord.y = origin.y + cellHeight;
  }
  else vs_out.TexCoord.y = origin.y;

  if((texCoord & (1<<7)) != 0)
  {
    vs_out.TexCoord.x = origin.x + cellWidth;
  }
  else vs_out.TexCoord.x = origin.x;

  vs_out.FinNormal = normVec;
  vs_out.FragPos = position;

  gl_Position = projection*view*vec4(position, 1.0f);
}

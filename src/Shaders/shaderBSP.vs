#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in float packagef;
out vec2 TexCoord;
out vec2 TexCells;
out vec2 TexOrigin;
out vec3 FinNormal;
out vec3 FragPos;
out float ClipSpaceDepth;



uniform mat4 view;
uniform mat4 projection;

uniform int textureAtlasWidth;
uniform int textureAtlasHeight;
uniform int cellWidth;

void main()
{
  int package = floatBitsToInt(packagef);
  int norm = ((package >> 24) & 0x3F);
  int texCoord = ((package >> 24) & 0xC0);
  int texId = (package >> 16) & 0xFF;
  int yblocks = (package >> 8) & 0xFF;
  int xblocks = package & 0xFF;

  int widthOfAtlasInCells = textureAtlasWidth/cellWidth;
  int heightOfAtlasInCells = textureAtlasHeight/cellWidth;
  float cellWidth = 1.0f/widthOfAtlasInCells;
  float cellHeight = 1.0f/heightOfAtlasInCells;
  vec3 normVec = vec3(0.0f,1.0f,0.0f);

  TexCells.x = xblocks;
  TexCells.y = yblocks;
  switch(norm)
  {
    //Front
    case(1<<0):
      normVec = vec3(1.0f,0.0f,0.0f);
      break;
    //Back
    case(1<<1):
      normVec = vec3(-1.0f,0.0f,0.0f);
      break;
    //Top
    case(1<<2):
      normVec = vec3(0.0f,1.0f,0.0f);
      break;
    //Bottom
    case(1<<3):
      normVec = vec3(0.0f,-1.0f,0.0f);
      break;
    //Left
    case(1<<4):
      normVec = vec3(0.0f,0.0f,1.0f);
      break;
    //Right
    case(1<<5):
      normVec = vec3(0.0f,0.0f,-1.0f);
      break;
  }


  vec2 origin;
  origin.x = (texId % widthOfAtlasInCells)*cellWidth;
  origin.y = (texId/widthOfAtlasInCells)*cellHeight;

  TexOrigin = origin;
  //origin = vec2(0.0f,0.0f);
  if((texCoord & (1<<6)) != 0)
  {
    TexCoord.y = origin.y + cellHeight;
  }
  else TexCoord.y = origin.y;

  if((texCoord & (1<<7)) != 0)
  {
    TexCoord.x = origin.x + cellWidth;
  }
  else TexCoord.x = origin.x;

  FinNormal = normVec;
  FragPos = position;
  gl_Position = projection*view*vec4(position, 1.0f);
  ClipSpaceDepth = gl_Position.z;
}

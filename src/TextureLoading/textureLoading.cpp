#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char* loadTexture(const char* texturePath,int* texWidth, int* texHeight,int* nrChannels)
{
  return stbi_load(texturePath, texWidth,texHeight,nrChannels, 0);
}

void freeTexture(unsigned char* texture)
{
  stbi_image_free(texture);
}

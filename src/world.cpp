#include <string>
#include "headers/SOIL.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <thread>
#include <queue>
// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

//Add the shader configs
#include "headers/shaders.h"
#include "headers/camera.h"

#include "headers/bsp.h"



World::World()
{

  int seed = 1737;

  perlin = new siv::PerlinNoise(seed);

  blockShader = new Shader("../src/shaders/shaderBSP.vs","../src/shaders/shaderBSP.fs");
  const char* texture = "../assets/textures/atlas.png";
  loadDictionary("../assets/dictionary.dat");

  frontNode = NULL;
  horzRenderDistance = 7;
  vertRenderDistance = 7;
  totalChunks = 0;
  lightposx = -10.0f;
  lightposy = 10.0f;
  lightposz = 0.0f;

  glGenTextures(1, &glTexture);
  glBindTexture(GL_TEXTURE_2D, glTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  //Load and bind the texture from the class
  int texWidth, texHeight;
  unsigned char* image = SOIL_load_image(texture, &texWidth, &texHeight, 0 , SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,0,GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);


}

World::~World()
{
  delete blockShader;
  delete perlin;

}


void World::generateChunk(int chunkx, int chunky, int chunkz)
{
  if(chunkExists(chunkx,chunky,chunkz)) return;

  std::shared_ptr<BSPNode>  tempChunk(new BSPNode(blockShader,dictionary, &glTexture,chunkx,chunky,chunkz,perlin));
  BSPmap[chunkx][chunky][chunkz] = tempChunk;

  if(frontNode == NULL)
  {
    frontNode = tempChunk;
  }
  else
  {
    tempChunk->nextNode = frontNode;
    frontNode->prevNode = tempChunk;
    frontNode = tempChunk;
  }


  std::shared_ptr<BSPNode>  otherChunk = getChunk(chunkx,chunky,chunkz-1);
  if(otherChunk  != NULL )
  {
    tempChunk->frontChunk = otherChunk;
    otherChunk->backChunk = tempChunk;
    otherChunk->toBuild = true;
  }

  otherChunk = getChunk(chunkx,chunky,chunkz+1);
  if(otherChunk != NULL)
  {
    tempChunk->backChunk = otherChunk;
    otherChunk->frontChunk = tempChunk;
    otherChunk->toBuild = true;
  }

  otherChunk = getChunk(chunkx,chunky-1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->bottomChunk = otherChunk;
    otherChunk->topChunk = tempChunk;
    otherChunk->toBuild = true;
  }

  otherChunk = getChunk(chunkx,chunky+1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->topChunk = otherChunk;
    otherChunk->bottomChunk = tempChunk;
    otherChunk->toBuild = true;
  }

  otherChunk = getChunk(chunkx-1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->leftChunk = otherChunk;
    otherChunk->rightChunk = tempChunk;
    otherChunk->toBuild = true;
  }

  otherChunk = getChunk(chunkx+1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->rightChunk = otherChunk;
    otherChunk->leftChunk = tempChunk;
    otherChunk->toBuild = true;
  }
  tempChunk->toBuild = true;
}

void World::renderWorld(float* mainx, float* mainy, float* mainz)
{



  for(int curDis = 0; curDis < horzRenderDistance; curDis++)
  {
    for(int height = 0; height < vertRenderDistance*2; height++)
    {
      int x = round(*mainx);
      int y = round(*mainy);
      int z = round(*mainz);
      x/= CHUNKSIZE;
      y/= CHUNKSIZE;
      z/= CHUNKSIZE;
      y--;

      int ychunk;
      if(height % 2 == 1) ychunk = y -(height/2);
      else ychunk = y + (height/2);

      int xchunk = x;
      int zchunk = z;


      zchunk = z+curDis;
      xchunk = x;
      for(int i = -curDis+1;i<curDis;i++)
      {
        generateChunk(xchunk+i,ychunk,zchunk);
      }

      zchunk = z-curDis;
      xchunk = x;
      for(int i = -curDis+1;i<curDis;i++)
      {
        generateChunk(xchunk+i,ychunk,zchunk);
      }
      zchunk = z;
      xchunk = x+curDis;
      for(int i = -curDis;i<=curDis;i++)
      {
        generateChunk(xchunk,ychunk,zchunk+i);
      }
      zchunk = z;
      xchunk = x-curDis;
      for(int i = -curDis;i<=curDis;i++)
      {
        generateChunk(xchunk,ychunk,zchunk+i);
      }
    }
  }
}

void World::buildWorld()
{
  std::shared_ptr<BSPNode>  curNode = frontNode;
  while(curNode != NULL)
  {
    if(curNode->toBuild == true)
    {
      curNode->build(this);
      curNode->toBuild = false;
    }
    curNode = curNode->nextNode;
  }

}


std::shared_ptr<BSPNode>  World::getChunk(int x, int y, int z)
{
  if(BSPmap.count(x) == 1)
   {
     if(BSPmap[x].count(y) == 1)
     {
       if(BSPmap[x][y].count(z) == 1)
       {
         return BSPmap[x][y][z];
       }
     }
   }
  return NULL;
}

void World::delChunk(int x, int y, int z)
{
  std::shared_ptr<BSPNode>  tempChunk = getChunk(x,y,z);
  if(tempChunk != NULL)
  {
    BSPmap[x][y].erase(z);
    if(BSPmap[x][y].empty())
    {
      BSPmap[x].erase(y);
      if(BSPmap[x].empty())
      {
        BSPmap.erase(x);
      }
    }

    /*
    tempChunk->toDelete = true;
    if(tempChunk->nextNode != NULL) tempChunk->nextNode->prevNode = tempChunk->prevNode;
    if(tempChunk->prevNode != NULL) tempChunk->prevNode->nextNode = tempChunk->nextNode;
    else frontNode = tempChunk->nextNode;
    */
    tempChunk->toDelete = true;
    if(tempChunk->leftChunk != NULL) tempChunk->leftChunk->rightChunk = NULL;
    if(tempChunk->rightChunk != NULL) tempChunk->rightChunk->leftChunk = NULL;
    if(tempChunk->frontChunk != NULL) tempChunk->frontChunk->backChunk = NULL;
    if(tempChunk->backChunk != NULL) tempChunk->backChunk->frontChunk = NULL;
    if(tempChunk->topChunk != NULL) tempChunk->topChunk->bottomChunk = NULL;
    if(tempChunk->bottomChunk != NULL) tempChunk->bottomChunk->topChunk = NULL;

    /*
    if(frontDelNode == NULL)
    {
      frontDelNode = tempChunk;
    }
    else
    {
      tempChunk->nextNode = frontNode;
      frontDelNode->prevNode = tempChunk;
      frontDelNode = tempChunk;
    }
    */
  }
}

void World::drawWorld(Camera* camera)
{
  std::shared_ptr<BSPNode>  curNode = frontNode;
  std::shared_ptr<BSPNode>  nextNode;
  while(curNode != NULL)
  {
    nextNode = curNode->nextNode;
    if(curNode->toDelete == true)
    {
      if(curNode->nextNode != NULL) curNode->nextNode->prevNode = curNode->prevNode;
      if(curNode->prevNode != NULL) curNode->prevNode->nextNode = curNode->nextNode;
      else frontNode = curNode->nextNode;

      curNode->nextNode = NULL;
      curNode->prevNode = NULL;
      curNode->curBSP.freeGL();

      std::cout << curNode.use_count() << "\n";

    }
    else curNode->draw(camera,lightposx,lightposy,lightposz);

    curNode = nextNode;
  }
}

void World::delScan(float* mainx, float* mainy, float* mainz)
{
  std::shared_ptr<BSPNode>  curNode = frontNode;
  while(curNode != NULL)
  {
    int x = round(*mainx);
    int y = round(*mainy);
    int z = round(*mainz);

    x/= CHUNKSIZE;
    y/= CHUNKSIZE;
    z/= CHUNKSIZE;

    int chunkx = curNode->curBSP.xCoord;
    int chunky = curNode->curBSP.yCoord;
    int chunkz = curNode->curBSP.zCoord;

    curNode = curNode->nextNode;
    if(sqrt(pow(chunkx-x,2)+pow(chunky-y,2)+pow(chunkz-z,2)) > horzRenderDistance*2 + 2)
    {
      delChunk(chunkx,chunky,chunkz);
    }
  }
}

bool World::chunkExists(int x ,int y, int z)
{
  if(BSPmap.count(x) == 1)
   {
     if(BSPmap[x].count(y) == 1)
     {
       if(BSPmap[x][y].count(z) == 1)
       {
         return true;
       }
     }
   }
  return false;
}

bool World::blockExists(int x, int y, int z)
{
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE)xlocal = 0;
  if(ylocal == CHUNKSIZE)ylocal = 0;
  if(zlocal == CHUNKSIZE)zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);

  std::shared_ptr<BSPNode>  tempChunk;
  if(tempChunk = getChunk(xchunk,ychunk,zchunk))
  {

    if(tempChunk->blockExists(xlocal,ylocal, zlocal))
    {
      return true;
    }
  }
  return false;
}

Block::Block(int newId, int* array, int newWidth,int newHeight,int newAtlasWidth, int newAtlasHeight)
{
  id = newId;
  for(int x=0;x<12;x++)
  {
    texArray[x] = array[x];
  }
  width = newWidth;
  height = newHeight;
  atlasWidth = newAtlasWidth;
  atlasHeight = newAtlasHeight;
}

bool World::loadDictionary(const char* file)
{
  dictionary = new Block*[5];
  int id = 0;
  using namespace std;
  string line;
  ifstream dictionaryf (file);
  if(dictionaryf.is_open())
  {
    getline(dictionaryf,line);
    int atlasWidth = stoi(line);
    getline(dictionaryf,line);
    int atlasHeight = stoi(line);

    int curBlock=0;
    while(getline(dictionaryf,line))
    {

      int texNumb;
      int texArray[12];
      stringstream ss;
      ss << line;
      for(int x = 0;x<12;x++)
      {
        ss >> texNumb;
        texArray[x] = texNumb;
        if(ss.peek() == ',') ss.ignore();
      }
      getline(dictionaryf,line);
      int width = stoi(line);
      getline(dictionaryf,line);
      int height = stoi(line);
      dictionary[curBlock] = new Block(id++,texArray,width,height,atlasWidth,atlasHeight);
      curBlock++;
    }
  }
}

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



World::World(int numbBuildThreads)
{
  typedef std::queue<std::shared_ptr<BSPNode>> buildType;
  buildQueue = new buildType[numbBuildThreads];
  numbOfThreads = numbBuildThreads;

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

void World::addToBuildQueue(std::shared_ptr<BSPNode> curNode)
{
  static int currentThread = 0;
  buildQueue[currentThread].push(curNode);
  curNode->toBuild = true;

  currentThread++;
  if(currentThread >= numbOfThreads) currentThread = 0;
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
  /*
    At this point the chunk is in both the linked list as well as the unorderedmap
    now it will attempt to find any nearby chunks and store a refrence to it
  */

  std::shared_ptr<BSPNode>  otherChunk = getChunk(chunkx,chunky,chunkz-1);
  if(otherChunk  != NULL )
  {
    tempChunk->frontChunk = otherChunk;
    otherChunk->backChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky,chunkz+1);
  if(otherChunk != NULL)
  {
    tempChunk->backChunk = otherChunk;
    otherChunk->frontChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky-1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->bottomChunk = otherChunk;
    otherChunk->topChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky+1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->topChunk = otherChunk;
    otherChunk->bottomChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx-1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->leftChunk = otherChunk;
    otherChunk->rightChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx+1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->rightChunk = otherChunk;
    otherChunk->leftChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);;
    }
  }
  //Adds the chunk to the current mainBuildQueue if it is not already;
  if(!tempChunk->toBuild)
  {
      addToBuildQueue(tempChunk);
  }
}

void World::renderWorld(float* mainx, float* mainy, float* mainz)
{
  //Dynamically renders world around the player prioritizing y
  //Does so in a ring like manner
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

void World::buildWorld(int threadNumb)
{
  //Empties the build queue
  //TODO Break up queue into smaller pieces and use seperate threads to build them
  while(!buildQueue[threadNumb].empty())
  {
    buildQueue[threadNumb].front()->build(this);
    buildQueue[threadNumb].pop();
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


void World::drawWorld(Camera* camera)
{
  /*
  Iterates through all the chunks and draws them unless they're marked for destruciton
  then it calls freeGl which will free up all opengl resourses on the chunk and then
  removes all refrences to it
  At that point the chunk should be deleted by the smart pointers;
  */
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
    Essentially marks the chunk for death
    Removes all connected pointers besides the linked list
    The Node however must be destroyed in the draw string
    since that is the sole string using opengl functions;
    othereise the vao wont get freed causing a memory leak
    */
    tempChunk->toDelete = true;
    if(tempChunk->leftChunk != NULL) tempChunk->leftChunk->rightChunk = NULL;
    if(tempChunk->rightChunk != NULL) tempChunk->rightChunk->leftChunk = NULL;
    if(tempChunk->frontChunk != NULL) tempChunk->frontChunk->backChunk = NULL;
    if(tempChunk->backChunk != NULL) tempChunk->backChunk->frontChunk = NULL;
    if(tempChunk->topChunk != NULL) tempChunk->topChunk->bottomChunk = NULL;
    if(tempChunk->bottomChunk != NULL) tempChunk->bottomChunk->topChunk = NULL;

  }
}

void World::delScan(float* mainx, float* mainy, float* mainz)
{
  /*
    Scans through the same list as the draw function
    however this time referencing the current position of the player
    and raises the destroy flag if its out of a certain range
  */
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
    if(abs(chunkx-x) > horzRenderDistance + 1
    || abs(chunky-y) > horzRenderDistance + 1
    || abs(chunkz-z) > horzRenderDistance + 1)
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
  /*
  Finds which ever chunk holds the block and then calls the blockExists
  function on that chunk with the localized coordinates
  */
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
  /*
  Loads the dictionary in such a format
  TODO: work on improving and discribing that format


  */
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


#include <string>
#include <fstream>
#include <sstream>

#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "include/bsp.h"
#include "../Inventory/include/items.h"

int BSPNode::totalChunks = 0;
bool BSP::geometryChanged = true;


BSPNode::BSPNode(int x,int y,int z,const std::string &wName)
{
  //std::cout << totalChunks << "\n";

  curBSP = BSP(x,y,z,wName);
  curBSP.loadFromFile();
  totalChunks++;
  toRender = false;
  toBuild = false;
  toDelete = false;
}

BSPNode::~BSPNode()
{
  totalChunks--;
}


void BSPNode::generateTerrain()
{
  curBSP.generateTerrain();
}



void BSPNode::saveChunk()
{
  curBSP.saveChunk();
}


void BSPNode::disconnect()
{
  BSPMutex.lock();
  toDelete = true;
  if(leftChunk != NULL) leftChunk->rightChunk = NULL;
  if(rightChunk != NULL) rightChunk->leftChunk = NULL;
  if(frontChunk != NULL) frontChunk->backChunk = NULL;
  if(backChunk != NULL) backChunk->frontChunk = NULL;
  if(topChunk != NULL) topChunk->bottomChunk = NULL;
  if(bottomChunk != NULL) bottomChunk->topChunk = NULL;

  BSPMutex.unlock();

}

bool BSPNode::blockExists(int x, int y, int z)
{
  return curBSP.blockExists(x, y, z);
}

void BSPNode::delBlock(int x, int y, int z)
{
  curBSP.delBlock(x, y, z);
}

void BSPNode::addBlock(int x, int y, int z, int id)
{
  curBSP.addBlock(x,y,z,id);
}

std::shared_ptr<std::string> BSPNode::getCompressedChunk()
{
  return curBSP.getCompressedChunk();
}


BSP::BSP(int x, int y, int z,const std::string &wName)
{
    xCoord = x;
    yCoord = y;
    zCoord = z;
    worldName = wName;
    //Intialize Buffers
}

bool BSP::loadFromFile()
{
  using namespace std;
  //The directoy to the chunk to be saved
  string directory = "saves/" + worldName + "/chunks/";
  string chunkName = to_string(xCoord) + '_' + to_string(yCoord) + '_' + to_string(zCoord) + ".dat";
  string chunkPath = directory+chunkName;

  //Max size of a chunk
  int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
  ifstream ichunk(chunkPath,ios::binary);

  //Checks if the file exists
  if(ichunk.is_open())
  {
    int i = 0;
    char curId=0;
    unsigned short curLength=0;
    while(i<numbOfBlocks)
    {

      ichunk.read((char*) &curId,sizeof(curId));
      ichunk.read((char*) &curLength,sizeof(curLength));

      for(int j = 0; j<curLength; j++)
      {
        worldArray[i+j] = curId;
      }
      i+= curLength;
    }
    ichunk.close();
    return true;
  }
  else
  {
    generateTerrain();
    return false;
  }


}

std::shared_ptr<std::string> BSP::getCompressedChunk()
{
    using namespace std;
    ostringstream chunk(ios::binary);
    unsigned short curTotal = 1;
    const int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
    char lastId = worldArray[0];
    for(int i = 1; i<numbOfBlocks;i++)
    {
      if(lastId == worldArray[i])
      {
        curTotal++;
      }
      else
      {
        chunk.write((char*) &lastId,sizeof(lastId));
        chunk.write((char*) &curTotal,sizeof(curTotal));
        curTotal = 1;
        lastId = worldArray[i];
      }
    }
    chunk.write((char*) &lastId,sizeof(lastId));
    chunk.write((char*) &curTotal,sizeof(curTotal));
    std::shared_ptr<std::string> temp = std::make_shared<std::string>(chunk.str());
    return temp;
}

inline void BSP::saveChunk()
{
  std::cout << "Saving chunk lmao\n";
  using namespace std;
  //The directoy to the chunk to be saved
  string directory = "saves/" + worldName + "/chunks/";
  string chunkName = to_string(xCoord) + '_' + to_string(yCoord) + '_' + to_string(zCoord) + ".dat";
  string chunkPath = directory+chunkName;

  //Max size of a chunk
  int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
  ofstream ochunk(chunkPath,ios::binary);
  unsigned short curTotal = 1;
  char lastId = worldArray[0];

  for(int i = 1; i<numbOfBlocks;i++)
  {
    if(lastId == worldArray[i])
    {
      curTotal++;
    }
    else
    {
      ochunk.write((char*) &lastId,sizeof(lastId));
      ochunk.write((char*) &curTotal,sizeof(curTotal));
      curTotal = 1;
      lastId = worldArray[i];
    }
  }
  ochunk.write((char*) &lastId,sizeof(lastId));
  ochunk.write((char*) &curTotal,sizeof(curTotal));
  ochunk.close();
}


inline int pack4chars(char a, char b, char c, char d)
{
  return ((a << 24) | (b << 16) | (c << 8) | d);
}


inline bool BSP::blockExists(int x, int y, int z)
{
  return worldArray.get(x,y,z) == 0 ? false : true;
}

void BSP::addBlock(int x, int y, int z, char id)
{
  worldArray.set(x,y,z,id);
}

void BSP::delBlock(int x, int y, int z)
{
  worldArray.set(x,y,z,0);

}

inline uchar BSP::getBlock(int x, int y, int z)
{
  return worldArray.get(x,y,z);
}

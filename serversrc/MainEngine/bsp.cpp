
#include <string>
#include <fstream>
#include <sstream>

#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "../headers/bsp.h"
#include "../headers/items.h"

int BSPNode::totalChunks = 0;
bool BSP::geometryChanged = true;


BSPNode::BSPNode(int x,int y,int z,const std::string &wName)
{
  //std::cout << totalChunks << "\n";

  curBSP = BSP(x,y,z,wName);
  totalChunks++;
  isGenerated = false;
  inUse = false;
  toRender = false;
  toBuild = false;
  toDelete = false;
}

BSPNode::~BSPNode()
{
  totalChunks--;
}

BSPNode::BSPNode(int x,int y, int z,const std::string &wName,const std::string &val)
{
  //std::cout << "generating chunk" << x << ":" << y << ":" << z << " with size" << val.size() << "\n";
  //std::cout << "curNumber of chunks: " << totalChunks << "\n";
  totalChunks++;
  BSPMutex.lock();
  curBSP = BSP(x,y,z,wName,val);
  isGenerated = false;
  inUse = false;
  toRender = false;
  toBuild = false;
  toDelete = false;
  BSPMutex.unlock();
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

BSP::BSP(int x, int y, int z,const std::string &wName,const std::string &val)
{



  xCoord = x;
  yCoord = y;
  zCoord = z;
  using namespace std;
  worldName = wName;

  const int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
  int i = 0;
  char curId = 0;
  unsigned short curLength = 0;
  const char* data = val.c_str();
  unsigned int index=0;

  while(i<numbOfBlocks)
  {
    curId = data[index];
    curLength = 0;
    index++;
    for(int j=0;j<sizeof(curLength);j++)
    {
      curLength += ((uchar) data[index] << (j*8));
      index++;
    }
    for(int j = 0; j<curLength; j++)
    {
      if(i+j>numbOfBlocks)
      {
        std::cout << "ERROR CORRUPTED CHUNK AT " << x << ":" << y << ":" << z <<"\n";
      }
      worldArray[i+j] = curId;
    }
    i+= curLength;
  }
}

BSP::BSP(int x, int y, int z,const std::string &wName)
{
    xCoord = x;
    yCoord = y;
    zCoord = z;
    worldName = wName;
    //Intialize Buffers

    using namespace std;
    //The directoy to the chunk to be saved
    string directory = "saves/" + worldName + "/chunks/";
    string chunkName = to_string(x) + '_' + to_string(y) + '_' + to_string(z) + ".dat";
    string chunkPath = directory+chunkName;

    //Max size of a chunk
    int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
    ifstream ichunk(chunkPath,ios::binary);

    //Checks if the file exists
    if(!ichunk.is_open())
    {

      generateTerrain();
      ichunk.close();
      ofstream ochunk(chunkPath,ios::binary);

      string compressed = *getCompressedChunk();
      ochunk << compressed;
      ochunk.close();

    }
    else
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
    }
}

std::shared_ptr<std::string> BSP::getCompressedChunk()
{
    using namespace std;
    ostringstream chunk(ios::binary);
    unsigned short curTotal = 1;
    int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
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

inline void BSP::delBlock(int x, int y, int z)
{
  worldArray.set(x,y,z,0);

}

inline uchar BSP::getBlock(int x, int y, int z)
{
  return worldArray.get(x,y,z);
}

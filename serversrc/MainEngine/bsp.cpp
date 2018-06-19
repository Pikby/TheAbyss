#include <fstream>
#include <sstream>
#include "../headers/bsp.h"


int totalChunks;


BSPNode::BSPNode(int x, int y, int z, std::string wName)
{
  //std::cout << totalChunks << "\n";
  curBSP = BSP(x,y,z,wName);

  isGenerated = false;
  nextNode = NULL;
  prevNode = NULL;
  toDelete = false;
  totalChunks++;
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

std::shared_ptr<std::string> BSPNode::getCompressedChunk()
{
  return curBSP.compressChunk();
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

int BSPNode::blockVisibleType(int x, int y, int z)
{
  return curBSP.blockVisibleType(x, y, z);
}

BSP::BSP(){}

BSP::BSP(int x, int y,int z,std::string wName)
{
    xCoord = x;
    yCoord = y;
    zCoord = z;
    for(int x = 0;x<CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;x++) worldMap[x] = 0;
    //Intialize Buffers

    worldName = wName;

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

      string compressed = *compressChunk();
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
          worldMap[i+j] = curId;
        }
        i+= curLength;
      }
      ichunk.close();
    }
}

BSP::~BSP()
{
  saveChunk();
}


inline std::shared_ptr<std::string> BSP::compressChunk()
{
    using namespace std;
    ostringstream chunk(ios::binary);
    unsigned short curTotal = 1;
    int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
    char lastId = worldMap[0];
    for(int i = 1; i<numbOfBlocks;i++)
    {
      if(lastId == worldMap[i])
      {
        curTotal++;
      }
      else
      {
        chunk.write((char*) &lastId,sizeof(lastId));
        chunk.write((char*) &curTotal,sizeof(curTotal));
        curTotal = 1;
        lastId = worldMap[i];
      }
    }
    chunk.write((char*) &lastId,sizeof(lastId));
    chunk.write((char*) &curTotal,sizeof(curTotal));
    std::shared_ptr<std::string> temp = std::make_shared<std::string>(chunk.str());
    return temp;

}

inline void BSP::saveChunk()
{
  using namespace std;
  //The directoy to the chunk to be saved
  string directory = "saves/" + worldName + "/chunks/";
  string chunkName = to_string(xCoord) + '_' + to_string(yCoord) + '_' + to_string(zCoord) + ".dat";
  string chunkPath = directory+chunkName;

  //Max size of a chunk
  int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
  ofstream ochunk(chunkPath,ios::binary);
  unsigned short curTotal = 1;
  char lastId = worldMap[0];

  for(int i = 1; i<numbOfBlocks;i++)
  {
    if(lastId == worldMap[i])
    {
      curTotal++;
    }
    else
    {
      ochunk.write((char*) &lastId,sizeof(lastId));
      ochunk.write((char*) &curTotal,sizeof(curTotal));
      curTotal = 1;
      lastId = worldMap[i];
    }
  }
  ochunk.write((char*) &lastId,sizeof(lastId));
  ochunk.write((char*) &curTotal,sizeof(curTotal));
  ochunk.close();
}



inline bool BSP::blockExists(int x, int y, int z)
{
  int id = worldMap[(int)x+CHUNKSIZE*(int)y+(int)z*CHUNKSIZE*CHUNKSIZE];
  if(id == 0 ) return false;
  else return true;
}

inline int BSP::blockVisibleType(int x, int y, int z)
{
  return ItemDatabase::blockDictionary[getBlock(x,y,z)].visibleType;
}

void BSP::addBlock(int x, int y, int z, char id)
{
  worldMap[x+y*CHUNKSIZE+z*CHUNKSIZE*CHUNKSIZE] = id;
}

inline void BSP::delBlock(int x, int y, int z)
{
  worldMap[x + y*CHUNKSIZE + z*CHUNKSIZE*CHUNKSIZE] = 0;

}

inline int BSP::getBlock(int x, int y, int z)
{
  return worldMap[x+y*CHUNKSIZE+z*CHUNKSIZE*CHUNKSIZE];
}

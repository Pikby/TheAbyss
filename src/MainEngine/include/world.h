
#pragma once
typedef unsigned char uchar;
typedef unsigned int uint;
#define CHUNKSIZE 32

#include <memory>
#include <mutex>
#include <map>
#include <queue>
#include <glm/glm.hpp>
#include "../../Objects/include/objects.h"
#include "messenger.h"
#include "drawer.h"
#include "../../headers/threadSafeQueue.h"
#include "../../headers/3dmap.h"



//Class which encapsulates all the chunks as well as the shaders and dicionary
class BSPNode;
class BSP;
class Block;
class MainChar;
class Player;
enum target{BLOCK = 0, NOTHING = -1};
class World
{
private:
  glm::ivec3 inline toLocalCoords(glm::ivec3 in);
  glm::ivec3 inline toChunkCoords(glm::ivec3 in);
  inline void checkForUpdates(glm::ivec3 local,std::shared_ptr<BSPNode> chunk);
public:
  Messenger messenger;
  Drawer drawer;
  Map3D<std::shared_ptr<BSPNode>> BSPmap;
  char mainId;

  TSafeQueue<std::shared_ptr<BSPNode>>* buildQueue;
  TSafeQueue<std::shared_ptr<BSPNode>> chunkDeleteQueue;
  int horzRenderDistance,vertRenderDistance,renderBuffer;
  int drawnChunks;
  int numbOfThreads;

  //The name of the world, for saving and loading
  std::string worldName;

  void calculateViewableChunks();
  bool blockExists(int x, int y, int z);
  bool blockExists(glm::vec3 pos)
  {
    return blockExists((int)floor(pos.x),(int)floor(pos.y),(int)floor(pos.z));
  }
  bool blockExists(double x,double y, double z)
  {
    return blockExists((int)floor(x),(int)floor(y),(int)floor(z));
  }
  int anyExists(glm::vec3 pos);
  bool entityExists(glm::vec3 pos);
  bool entityExists(float x, float y, float z);

  std::shared_ptr<BSPNode> getChunk(int x, int y, int z);
  glm::vec4 rayCast(glm::vec3 pos, glm::vec3 front, int max);

  void addToBuildQueue(std::shared_ptr<BSPNode> curNode);
  World(int numbBuildThreads,int width,int height);

  void renderWorld(float mainx, float mainy, float mainz);

  void buildWorld(int threadNumb);
  bool chunkExists(int x, int y, int z);
  void addPlayer(float x, float y, float z, uchar id);
  void removePlayer(uchar id);
  void movePlayer(float x,float y, float z, uchar id);

  void delBlock(int x, int y, int z);
  void delChunk(int x, int y, int z);
  void deleteChunksFromQueue();
  void addBlock(int x, int y, int z, int id);
  void updateBlock(int x, int y, int z);
  void delScan(float mainx, float mainy, float mainz);
  void generateChunkFromString(int chunkx, int chunky, int chunkz,const std::string &val);

};

//The class for each individual block in the dictionary

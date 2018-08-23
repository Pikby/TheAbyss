
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
  static glm::ivec3 toLocalCoords(glm::ivec3 in);
  static glm::ivec3 toChunkCoords(glm::ivec3 in);
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
  bool blockExists(const glm::ivec3 &pos);
  bool blockExists(const glm::vec3 &pos)
  {
    return blockExists(glm::ivec3(floor(pos)));
  }
  int anyExists(const glm::vec3 &pos);
  bool entityExists(const glm::vec3 &pos);

  std::shared_ptr<BSPNode> getChunk(const glm::ivec3 &pos);
  std::shared_ptr<BSPNode> chunkRayCast(const glm::vec3 &pos, const glm::vec3 &front);
  glm::vec4 rayCast(glm::vec3 pos, glm::vec3 front, int max);

  void addToBuildQueue(std::shared_ptr<BSPNode> curNode);
  void addToBuildQueueFront(std::shared_ptr<BSPNode> curNode);
  World(int numbBuildThreads,int width,int height);

  void renderWorld(float mainx, float mainy, float mainz);

  void buildWorld(int threadNumb);
  bool chunkExists(const glm::ivec3 &pos);
  void addPlayer(const glm::vec3 &pos, uchar id);
  void removePlayer(uchar id);
  void movePlayer(const glm::vec3 &pos, uchar id);

  void delBlock(const glm::ivec3 &pos);
  void delChunk(const glm::ivec3 &pos);
  void deleteChunksFromQueue();
  void findChunkToRequest(const float mainx, const float mainy, const float mainz);
  void addBlock(const glm::ivec3 &pos, uchar id);
  void updateBlock(const glm::ivec3 &pos);
  void delScan(float mainx, float mainy, float mainz);
  void generateChunkFromString(const glm::ivec3 &pos,const char* val);

};

//The class for each individual block in the dictionary

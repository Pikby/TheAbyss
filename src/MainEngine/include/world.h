#ifndef WORLDHEADER
#define WOLRDHEADER

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
class MainChar;
class Player;
enum target{BLOCK = 0, NOTHING = -1};
class World
{
private:
  static glm::ivec3 toLocalCoords(const glm::ivec3 &in);
  static glm::ivec3 toChunkCoords(const glm::ivec3 &in);
  static inline void checkForUpdates(const glm::ivec3 &local,std::shared_ptr<BSPNode> chunk);
public:
  static Messenger messenger;
  static Drawer drawer;
  static Map3D<std::shared_ptr<BSPNode>> BSPmap;
  static char mainId;

  static TSafeQueue<std::shared_ptr<BSPNode>> buildQueue;
  static TSafeQueue<std::shared_ptr<BSPNode>> chunkDeleteQueue;
  static int horzRenderDistance,vertRenderDistance,renderBuffer;
  static int drawnChunks;
  static int numbOfBuildThreads;

  //The name of the world, for saving and loading
  static std::string worldName;
  static void calculateViewableChunks();
  static bool blockExists(const glm::ivec3 &pos);
  static bool blockExists(const glm::vec3 &pos)
  {
    return blockExists(glm::ivec3(floor(pos)));
  }

  static int anyExists(const glm::vec3 &pos);
  static bool entityExists(const glm::vec3 &pos);

  static std::shared_ptr<BSPNode> getChunk(const glm::ivec3 &pos);
  static std::shared_ptr<BSPNode> chunkRayCast(const glm::vec3 &pos, const glm::vec3 &front);
  static glm::vec4 rayCast(const glm::vec3 &pos,const glm::vec3 &front, int max);

  static void addToBuildQueue(std::shared_ptr<BSPNode> curNode);
  static void addToBuildQueueFront(std::shared_ptr<BSPNode> curNode);
  static void initWorld(int numbBuildThreads,int width,int height);

  static void renderWorld(const glm::vec3& pos);

  static void buildWorld(char threadNumb);
  static bool chunkExists(const glm::ivec3 &pos);
  static void addPlayer(const glm::vec3 &pos, uchar id);
  static void removePlayer(uchar id);
  static void movePlayer(const glm::vec3 &pos, uchar id);

  static std::list<Light> findAllLights(const glm::vec3 &playerPos,int count);
  static void delBlock(const glm::ivec3 &pos);
  static void delChunk(const glm::ivec3 &pos);
  static void deleteChunksFromQueue();
  static void findChunkToRequest(const float mainx, const float mainy, const float mainz);
  static void addBlock(const glm::ivec3 &pos, uchar id);
  static void updateBlock(const glm::ivec3 &pos);
  static void delScan(const glm::vec3& pos);
  static void generateChunkFromString(const glm::ivec3 &pos,const char* val);
};


#ifdef WORLDIMPLEMENTATION
Messenger World::messenger;
Drawer World::drawer;
Map3D<std::shared_ptr<BSPNode>> World::BSPmap;
char World::mainId;
TSafeQueue<std::shared_ptr<BSPNode>> World::buildQueue;
TSafeQueue<std::shared_ptr<BSPNode>> World::chunkDeleteQueue;
int World::horzRenderDistance,World::vertRenderDistance,World::renderBuffer;
int World::drawnChunks;
int World::numbOfBuildThreads;
std::string World::worldName;
#endif


#endif

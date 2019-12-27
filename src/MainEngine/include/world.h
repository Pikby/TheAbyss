#ifndef WORLDHEADER
#define WOLRDHEADER

#define CHUNKSIZE 32

#include <memory>
#include <mutex>
#include <map>
#include <queue>
#include <glm/glm.hpp>


#include "../../headers/shaders.h"
#include "../../headers/threadSafeQueue.h"
#include "../../headers/3dmap.h"
#include "../../headers/camera.h"


#include "messages.h"


struct WorldStats
{
  int chunksInMemory;
  int chunksInBuildQueue;

  double pingInMS;
};

//Class which encapsulates all the chunks as well as the shaders and dicionary
class BSPNode;
class MainChar;
class Player;
class Object;
class Cube;
class Drawer;
class Messenger;

enum target{BLOCK = 0, NOTHING = -1};
class World
{
private:
  void checkForUpdates(const glm::ivec3 &local,std::shared_ptr<BSPNode> chunk);
public:

  std::unique_ptr<Messenger>  messenger;
  std::unique_ptr<Drawer> drawer;
  WorldStats worldStats;

  Map3D<std::shared_ptr<BSPNode>> BSPmap;
  char mainId;

  TSafeQueue<std::shared_ptr<BSPNode>> buildQueue;
  TSafeQueue<std::shared_ptr<BSPNode>> chunkDeleteQueue;
  int horzRenderDistance,vertRenderDistance,renderBuffer;
  int drawnChunks;
  int numbOfBuildThreads;



  //The name of the world, for saving and loading
  std::string worldName;
  void initWorld();
  void connectToServer(const std::string& ip,const std::string& username);
  void destroyWorld();


  void calculateViewableChunks();
  bool blockExists(const glm::ivec3 &pos);
  bool blockExists(const glm::vec3 &pos){return blockExists(glm::ivec3(floor(pos)));}

  static glm::ivec3 toLocalCoords(const glm::ivec3 &in);
  static glm::ivec3 toChunkCoords(const glm::ivec3 &in);



  int anyExists(const glm::vec3 &pos);
  bool entityExists(const glm::vec3 &pos);

  std::shared_ptr<BSPNode> getChunk(const glm::ivec3 &pos);
  std::shared_ptr<BSPNode> chunkRayCast(const glm::vec3 &pos, const glm::vec3 &front);
  glm::vec4 rayCast(const glm::vec3 &pos,const glm::vec3 &front, int max);

  void addToBuildQueue(std::shared_ptr<BSPNode> curNode);
  void addToBuildQueueFront(std::shared_ptr<BSPNode> curNode);

  void renderWorld(const glm::vec3& pos);

  void buildWorld();
  bool chunkExists(const glm::ivec3 &pos);

  void addPlayer(const glm::vec3 &pos, uint8_t id);
  void removePlayer(uint8_t id);
  void movePlayer(const glm::vec3 &pos, uint8_t id);
  void updatePlayerViewDirection(const glm::vec3 &pos, uint8_t id);


  //std::list<BSP::Light> findAllLights(const glm::vec3 &playerPos,int count);
  void delBlock(const glm::ivec3 &pos);
  void delChunk(const glm::ivec3 &pos);
  void deleteChunksFromQueue();
  void findChunkToRequest(const float mainx, const float mainy, const float mainz);
  void addBlock(const glm::ivec3 &pos, uint8_t id);
  void updateBlock(const glm::ivec3 &pos);
  void drawPreviewBlock(const glm::ivec3 &pos);
  void delScan(const glm::vec3 &pos);
  void generateChunkFromString(const glm::ivec3 &pos,const char* val);
};
extern World PlayerWorld;
#endif

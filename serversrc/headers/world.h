#pragma once
#define CHUNKSIZE 32
#include "../headers/FastNoise.h"
#include "../headers/3dmap.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <memory>
#include <boost/filesystem.hpp>
#include <unordered_map>
//Class which encapsulates all the chunks as well as the shaders and dicionary
class BSPNode;
class BSP;
class Block;
class MainChar;


class World
{
  private:
  glm::ivec3 inline toChunkCoords(glm::ivec3 in);
  glm::ivec3 inline toLocalCoords(glm::ivec3 in);
  public:
  ~World(){saveWorld();}
  Map3D<std::shared_ptr<BSPNode>> BSPmap;
  unsigned int totalChunks;
  //global texture atlas
  //The seed of the world, for generation
  int seed;
  //Perlin noise object
  FastNoise perlin;
  //The name of the world, for saving and loading
  std::string worldName;
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
  enum target{BLOCK = 0, NOTHING = -1};
  glm::vec4 rayCast(glm::vec3 pos, glm::vec3 front, int max);
  void addToBuildQueue(std::shared_ptr<BSPNode> curNode);
  World();
  bool chunkExists(int x, int y, int z);
  void delBlock(int x, int y, int z);
  void delChunk(int x, int y, int z);
  void addBlock(int x, int y, int z, int id);
  void updateBlock(int x, int y, int z);
  void delScan(float* mainx, float* mainy, float* mainz);
  void generateChunk(int chunkx, int chunky, int chunkz);
  void saveWorld();
};

//The class for each individual block in the dictionary

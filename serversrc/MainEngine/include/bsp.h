#pragma once
#include <list>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>

#include "../../headers/3darray.h"
#include "../../TerrainGeneration/FastNoiseSIMD/FastNoiseSIMD.h"


typedef unsigned char uchar;
#define CHUNKSIZE 32

//Class which holds the data for each individual chunk
class BSPNode;

class BSP
{
private:
  std::string worldName;

  Array3D<uchar, CHUNKSIZE> worldArray;
  static FastNoiseSIMD* noise3d;
public:
  static bool geometryChanged;
  int xCoord,yCoord,zCoord;
  std::shared_ptr<std::string> getCompressedChunk();
  BSP(int x,int y,int z,const std::string &wName);
  BSP(int x,int y,int z,const std::string &wName, char* data);
  BSP(){};
  void generateTerrain();
  bool loadFromFile();
  static void initTerrainGenEngine();
  void addBlock(int x, int y, int z,char id);
  bool blockExists(int x,int y,int z);
  uchar getBlock(int x, int y, int z);
  void delBlock(int x, int y, int z);
  void saveChunk();
  std::string compressChunk();
  glm::vec3 offset(float x, float y,float z);
};

class BSPNode
{
  private:
  std::mutex BSPMutex;
  public:
  static int totalChunks;
  BSP curBSP;
  BSPNode(int x,int y,int z,const std::string &wName);
  BSPNode(int x,int y,int z,const std::string &wName,const std::string &val);
  ~BSPNode();
  void saveChunk();
  bool blockExists(int x, int y, int z);
  int blockVisibleType(int x, int y, int z);
  void generateTerrain();
  std::shared_ptr<std::string> getCompressedChunk();
  void delBlock(int x, int y, int z);
  void addBlock(int x, int y, int z, int id);
  void disconnect();

  //references to the 6 cardinal neighbours of the chunk
  std::shared_ptr<BSPNode>  leftChunk,rightChunk,frontChunk,backChunk,topChunk,bottomChunk;

};

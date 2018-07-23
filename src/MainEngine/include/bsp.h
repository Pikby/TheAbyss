#pragma once
#include <list>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include "../../headers/shaders.h"
#include "../../headers/3darray.h"

typedef unsigned char uchar;
#define CHUNKSIZE 32

//Class which holds the data for each individual chunk
class BSPNode;

class BSP
{
private:
  uchar curBlockid;
  char xdist;
  char ydist;
  enum Faces {FRONTF = 1 << 0, BACKF = 1 << 1,
              TOPF = 1 << 2, BOTTOMF = 1 << 3,
              LEFTF = 1 << 4, RIGHTF = 1 << 5};

  enum TextureSides {BOTTOM  = 0,TOP = 1 << 6, RIGHT = 1 << 7, LEFT = 0};
  struct BlockFace
  {
    char data;
    void setFace(Faces f)
    {
      data |= f;
    }
    void delFace(Faces f)
    {
      data &= ~f;
    }
    bool getFace(Faces f)
    {
      return (data & f) != 0 ? true : false;
    }
    BlockFace(){data = 0;}
  };
  //Opaque objects
  std::shared_ptr<std::vector<float>> oVertices;
  std::shared_ptr<std::vector<uint>> oIndices;
  uint oVBO, oEBO, oVAO;

  std::shared_ptr<std::vector<float>> oVerticesBuffer;
  std::shared_ptr<std::vector<uint>> oIndicesBuffer;
  //Translucent objects
  std::shared_ptr<std::vector<float>> tVertices;
  std::shared_ptr<std::vector<uint>> tIndices;

  std::shared_ptr<std::vector<float>> tVerticesBuffer;
  std::shared_ptr<std::vector<uint>> tIndicesBuffer;
  uint tVBO, tEBO, tVAO;
  std::string worldName;

  int addVertex(int renderType, const glm::vec3 &pos,Faces face, TextureSides texX, TextureSides texY);
  void addIndices(int renderType,int index1, int index2, int index3, int index4);

  Array3D<uchar, CHUNKSIZE> worldArray;

public:
  static bool geometryChanged;
  int xCoord;
  int yCoord;
  int zCoord;

  BSP(int x,int y,int z,const std::string &wName);
  BSP(int x,int y,int z,const std::string &wName, const std::string &val);
  BSP(){};
  void generateTerrain();
  void render();
  void addBlock(int x, int y, int z,char id);
  void freeGL();
  bool blockExists(int x,int y,int z);
  int blockVisibleType(int x, int y, int z);
  uchar getBlock(int x, int y, int z);
  void delBlock(int x, int y, int z);
  void saveChunk();
  std::string compressChunk();
  glm::vec3 offset(float x, float y,float z);

  void build(std::shared_ptr<BSPNode>  curRightChunk,std::shared_ptr<BSPNode>  curLeftChunk,std::shared_ptr<BSPNode>  curTopChunk,
                       std::shared_ptr<BSPNode>  curBottomChunk,std::shared_ptr<BSPNode>  curFrontChunk,std::shared_ptr<BSPNode>  curBackChunk);
  void drawOpaque();
  void drawTranslucent();

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
  void build();
  void drawOpaque();
  void drawTranslucent();
  void generateTerrain();
  std::string getCompressedChunk();
  void delBlock(int x, int y, int z);
  void addBlock(int x, int y, int z, int id);
  void del();
  void disconnect();

  //references to the 6 cardinal neighbours of the chunk
  std::shared_ptr<BSPNode>  leftChunk,rightChunk,frontChunk,backChunk,topChunk,bottomChunk;

  //Flags for use inbetween pointers
  std::atomic<bool> toRender,toBuild,toDelete,isGenerated,inUse;
};

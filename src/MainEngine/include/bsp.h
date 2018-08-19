#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <list>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include "../../headers/shaders.h"
#include "../../headers/3darray.h"
#include "../../Objects/include/items.h"
typedef unsigned char uchar;
#define CHUNKSIZE 32
//Class which holds the data for each individual chunk
class BSPNode;
enum AmbientOcclusion {NOAO = 0, SINGLE = 1, CONNECTED = 2, FULLCOVER = 3};
class BSP
{
private:
  BSPNode* parent;
  uchar curBlockid;
  char xdist;
  char ydist;
  glm::ivec3 curLocalPos;
  enum Faces {FRONTF = 1 << 0, BACKF = 1 << 1,
              TOPF = 1 << 2, BOTTOMF = 1 << 3,
              RIGHTF = 1 << 4, LEFTF = 1 << 5};

  enum TextureSides {BOTTOM  = 0,TOP = 1 << 6, RIGHT = 1 << 7, LEFT = 0};

  struct BlockFace
  {
    char faceData;
    void setFace(Faces f)
    {
      faceData |= f;
    }
    void delFace(Faces f)
    {
      faceData &= ~f;
    }
    bool getFace(Faces f)
    {
      return ((faceData & f) != 0);
    }
    BlockFace(){faceData = 0;}
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
  AmbientOcclusion getAO(glm::ivec3 pos, Faces face, TextureSides top, TextureSides right);
  int addVertex(RenderType renderType, const glm::vec3 &pos,Faces face, TextureSides texX, TextureSides texY, char* AOvalue);
  void addIndices(RenderType renderType,int index1, int index2, int index3, int index4);
  void calcFace(const int x, const int y, const int z, Array3D<BlockFace,32>* arrayFaces,
                char* outtop, char* outright, char blockId, Faces face,
                const glm::ivec3 &topVector, const glm::ivec3 &rightVector);
  void setupBufferObjects(RenderType type);
  Array3D<uchar, CHUNKSIZE> worldArray;

public:
  RenderType blockVisibleType(int x, int y, int z);

  static bool geometryChanged;
  int xCoord,yCoord,zCoord;

  BSP(int x,int y,int z,const std::string &wName,const char* data,BSPNode* Parent);
  BSP(){};
  void render();
  void addBlock(int x, int y, int z,char id);
  void freeGL();
  bool blockExists(int x,int y,int z);

  uchar getBlock(int x, int y, int z);
  void delBlock(int x, int y, int z);
  glm::vec3 offset(float x, float y,float z);

  void build();
  void drawOpaque();
  void drawTranslucent();
  void swapBuffers();
};

class BSPNode
{
  private:
  std::mutex BSPMutex;
  public:
  static int totalChunks;
  BSP curBSP;
  BSPNode(int x,int y,int z,const std::string &wName,const char* val);
  ~BSPNode();
  void saveChunk();
  bool blockExists(glm::ivec3 vec)
  {
    return blockExists(vec.x,vec.y,vec.z);
  }
  bool blockExists(int x, int y, int z);
  RenderType blockVisibleTypeOOB(int x,int y, int z);
  RenderType blockVisibleType(int x, int y, int z);
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
  std::atomic<bool> toRender,toBuild,toDelete;
};

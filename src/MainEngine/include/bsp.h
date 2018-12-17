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
  bool isEmpty()
  {
    return (faceData != 0);
  }
  BlockFace(){faceData = 0;}
};

class BSP
{
  private:
    static std::string worldName;
    Array3D<uchar, CHUNKSIZE> worldArray;
    BSPNode* parent;
    //Variables for use when adding new vertices to the stack
    uchar curBlockid;
    char xdist;
    char ydist;
    glm::ivec3 curLocalPos;

    enum TextureSides {BOTTOM  = 0,TOP = 1 << 6, RIGHT = 1 << 7, LEFT = 0};

    //The temporary vectors to hold the vertex data until its passed onto the GPU
    std::vector<float> oVertices,tVertices;
    std::vector<uint> oIndices,tIndices;
    //The stored number of indices so we know how many indices to draw
    int oIndicesSize,tIndicesSize;
    uint oVBO, oEBO, oVAO, tVBO, tEBO, tVAO;




    AmbientOcclusion getAO(const glm::ivec3 &pos, Faces face, TextureSides top, TextureSides right);
    int addVertex(RenderType renderType, const glm::vec3 &pos,Faces face, TextureSides texX, TextureSides texY, char* AOvalue);
    void addIndices(RenderType renderType,int index1, int index2, int index3, int index4);
    void setupBufferObjects(RenderType type);


  public:
    static bool geometryChanged;
    RenderType blockVisibleType(const glm::ivec3 &pos);


    BSP(const char* data,BSPNode* Parent);
    void render();
    void freeGL();
    void addBlock(const glm::ivec3 &pos,char id);
    bool blockExists(const glm::ivec3 &pos);
    uchar getBlock(const glm::ivec3 &pos);
    void delBlock(const glm::ivec3 &pos);

    void build();
    void drawOpaque();
    void drawTranslucent();
};

class BSPNode
{
  private:
    BSP curBSP;

    //references to the 6 cardinal neighbours of the chunk
    std::shared_ptr<BSPNode>  leftChunk,rightChunk,frontChunk,backChunk,topChunk,bottomChunk;
  public:
    std::recursive_mutex BSPMutex;
    static int totalChunks;
    glm::ivec3 chunkPos;

    //Flags for use inbetween pointers
    std::atomic<bool> toRender,toBuild,toDelete;

    BSPNode(const glm::ivec3 &pos,const char* val);
    ~BSPNode();
    void saveChunk();
    bool blockExists(const glm::ivec3 &pos);
    RenderType blockVisibleTypeOOB(const glm::ivec3 &pos);
    RenderType blockVisibleType(const glm::ivec3 &pos);


    void build();
    void drawOpaque();
    void drawTranslucent();
    std::string getCompressedChunk();
    void delBlock(const glm::ivec3 &pos);
    void addBlock(const glm::ivec3 &pos, char id);
    void del();
    void disconnect();
    glm::ivec3 getRealWorldPosition();
    uchar getBlock(const glm::ivec3 &pos);

    std::shared_ptr<BSPNode> getNeighbour(Faces face);
    void setNeighbour(Faces face, std::shared_ptr<BSPNode> neighbour);



};

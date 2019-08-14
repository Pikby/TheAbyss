#ifndef BSPHEADER
#define BSPHEADER

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

#include <memory>
#include <atomic>
#include <mutex>
#include <list>
#include <vector>
#include <map>

#include "../../headers/3darray.h"
#include "types.h"

class Shader;


typedef unsigned char uint8_t;
#define CHUNKSIZE 32
//Class which holds the data for each individual chunk
class BSPNode;
enum AmbientOcclusion {NOAO = 0, SINGLE = 1, CONNECTED = 2, FULLCOVER = 3};
enum TextureSides {BOTTOMTS  = 0,TOPTS = 1 << 6, RIGHTTS = 1 << 7, LEFTTS = 0};





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
    return (faceData == 0);
  }
  BlockFace(){faceData = 0;}
};

struct FaceData
{
  uint8_t blockId;
  RenderType renderType;
  glm::ivec3 pos;
  glm::ivec2 textCount;
  Faces face;
};

struct VertexData
{
  uint8_t texIds[3];
  uint8_t vId;
  RenderType renderType;
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec3 flatNorm;
  Faces face;
  AmbientOcclusion ao;
  TextureSides tb,rl;
};


struct Light
{
  glm::vec3 pos;
  glm::vec3 color;
  double radius;
};

class BSP
{
  private:
    std::map<int,Light> lightList;
    glm::mat4 modelMat;
    static std::string worldName;
    Array3D<uint8_t, CHUNKSIZE> worldArray;
    BSPNode* parent;


    //The temporary vectors to hold the vertex data until its passed onto the GPU
    std::vector<float> oVertices,tVertices;
    std::vector<uint> oIndices,tIndices;
    //The stored number of indices so we know how many indices to draw
    int oIndicesSize = 0,tIndicesSize = 0;
    uint oVBO = 0, oEBO = 0, oVAO = 0,
         tVBO = 0, tEBO = 0, tVAO = 0;


    AmbientOcclusion getAO(const glm::ivec3 &pos, Faces face, TextureSides top, TextureSides right);
    int addVertex(const VertexData& vertex);
    void addIndices(RenderType renderType,int index1, int index2, int index3, int index4);
    void setupBufferObjects(RenderType type);


  public:
    BSP(const char* data,const glm::ivec3 &pos,BSPNode* Parent);
    static bool geometryChanged;

    bool empty();
    void addToLightList(const glm::ivec3 &localPos,const Light& light);
    bool lightExists(const glm::ivec3 &localPos);
    void removeFromLightList(const glm::ivec3 &localPos);
    std::list<Light> getFromLightList(int count);

    void drawPreviewBlock(Shader* shader,const glm::ivec3& pos,const glm::vec3& viewpos);
    RenderType blockVisibleType(const glm::ivec3 &pos);
    void render();
    void freeGL();
    void addBlock(const glm::ivec3 &pos,char id);
    bool blockExists(const glm::ivec3 &pos);
    uint8_t getBlock(const glm::ivec3 &pos);
    void delBlock(const glm::ivec3 &pos);

    void build();
    void drawOpaque(Shader* shader,const glm::vec3 &viewPos);
    void drawTranslucent(Shader* shader, const glm::vec3 &viewPos);
    void drawChunkOutline(Shader* shader, const glm::vec3 &viewPos);
};

class BSPNode
{
  private:
    BSP curBSP;

    //references to the 6 cardinal neighbours of the chunk
    std::shared_ptr<BSPNode>  leftChunk = NULL,rightChunk = NULL,frontChunk = NULL,
                              backChunk = NULL,topChunk = NULL,bottomChunk = NULL;
    std::recursive_mutex BSPMutex;
    glm::ivec3 chunkPos;
  public:
    static int totalChunks;

    //Flags for use inbetween pointers
    std::atomic<bool> toRender = false,toBuild = false,toDelete = false;



    BSPNode(const glm::ivec3 &pos,const char* val);
    ~BSPNode();

    glm::ivec3 getPosition(){return chunkPos;}
    void saveChunk();
    bool blockExists(const glm::ivec3 &pos);
    RenderType blockVisibleType(const glm::ivec3 &pos);
    uint8_t getBlockOOB(const glm::ivec3 &pos);


    std::list<Light> getFromLightList(int count);
    void build();
    void drawOpaque(Shader* shader, const glm::vec3 &pos);
    void drawTranslucent(Shader* shader, const glm::vec3 &pos);
    void drawChunkOutline(Shader* shader, const glm::vec3 &pos);
    void drawPreviewBlock(Shader* shader,const glm::ivec3& pos,const glm::vec3& viewpos);

    std::string getCompressedChunk();
    void delBlock(const glm::ivec3 &pos);
    void addBlock(const glm::ivec3 &pos, uint8_t id);
    void del();
    void disconnect();
    glm::ivec3 getRealWorldPosition();
    uint8_t getBlock(const glm::ivec3 &pos);

    std::shared_ptr<BSPNode> getNeighbour(Faces face);
    void setNeighbour(Faces face, std::shared_ptr<BSPNode> neighbour);


};
#endif

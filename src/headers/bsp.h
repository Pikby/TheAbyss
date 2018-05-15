#include <list>
#include <memory>
#include "perlinnoise.h"
#include "world.h"
#include "items.h"


//Class which holds the data for each individual chunk
class BSP : public WorldWrap
{
private:

  //Opaque objects
  std::shared_ptr<std::vector<GLfloat>> oVertices;
  std::shared_ptr<std::vector<GLuint>> oIndices;
  GLuint oVBO, oEBO, oVAO;

  std::shared_ptr<std::vector<GLfloat>> oVerticesBuffer;
  std::shared_ptr<std::vector<GLuint>> oIndicesBuffer;
  //Translucent objects
  std::shared_ptr<std::vector<GLfloat>> tVertices;
  std::shared_ptr<std::vector<GLuint>> tIndices;

  std::shared_ptr<std::vector<GLfloat>> tVerticesBuffer;
  std::shared_ptr<std::vector<GLuint>> tIndicesBuffer;
  GLuint tVBO, tEBO, tVAO;

  int addVertex(int renderType, float x, float y, float z,float xn, float yn, float zn, float texX, float texY);
  void addIndices(int renderType,int index1, int index2, int index3, int index4);

  unsigned char worldMap[CHUNKSIZE*CHUNKSIZE*CHUNKSIZE];

public:
  int xCoord;
  int yCoord;
  int zCoord;

  BSP(int x,int y,int z);
  BSP(int x,int y,int z, std::string val);
  BSP();
  ~BSP();
  void generateTerrain();
  void render();
  void addBlock(int x, int y, int z,char id);
  void freeGL();
  bool blockExists(int x,int y,int z);
  int blockVisibleType(int x, int y, int z);
  int getBlock(int x, int y, int z);
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
  BSP curBSP;
  BSPNode(int x,int y,int z);
  BSPNode(int x,int y,int z,std::string val);
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
  //next and prev node for the linked list of all nodes
  std::shared_ptr<BSPNode>  nextNode;
  std::shared_ptr<BSPNode>  prevNode;

  //references to the 6 cardinal neighbours of the chunk
  std::shared_ptr<BSPNode>  leftChunk;
  std::shared_ptr<BSPNode>  rightChunk;
  std::shared_ptr<BSPNode>  frontChunk;
  std::shared_ptr<BSPNode>  backChunk;
  std::shared_ptr<BSPNode>  topChunk;
  std::shared_ptr<BSPNode>  bottomChunk;

  //Flags for use inbetween pointers
  bool toRender;
  bool toBuild;
  bool toDelete;
  bool isGenerated;
  bool inUse;
};

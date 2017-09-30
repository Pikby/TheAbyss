#include <list>
#include <memory>
#include "perlinnoise.h"
#include "world.h"

enum renderType {OPAQUE,TRANSLUCENT,TRANSPARENT};
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

  char worldMap[CHUNKSIZE*CHUNKSIZE*CHUNKSIZE];

public:

  long int xCoord;
  long int yCoord;
  long int zCoord;
  BSP(long int x, long int y, long int z);
  BSP();
  ~BSP();
  void generateTerrain(  siv::PerlinNoise* perlin);
  void render();
  void addBlock(int x, int y, int z,char id);
  void freeGL();
  bool blockExists(int x,int y,int z);
  int blockVisibleType(int x, int y, int z);
  int getBlock(int x, int y, int z);
  int removeBlock(int x, int y, int z);
  glm::vec3 offset(float x, float y,float z);
  void build(std::shared_ptr<BSPNode>  curRightChunk,std::shared_ptr<BSPNode>  curLeftChunk,std::shared_ptr<BSPNode>  curTopChunk,
                       std::shared_ptr<BSPNode>  curBottomChunk,std::shared_ptr<BSPNode>  curFrontChunk,std::shared_ptr<BSPNode>  curBackChunk);
  void drawOpaque(Camera* camera);
  void drawTranslucent(Camera* camera);

};

class BSPNode
{
  public:
  BSP curBSP;
  BSPNode(long int x, long int y, long int z);
  ~BSPNode();
  bool blockExists(int x, int y, int z);
  int blockVisibleType(int x, int y, int z);
  void build();
  void drawOpaque(Camera* camera);
  void drawTranslucent(Camera* camera);
  std::shared_ptr<BSPNode>  nextNode;
  std::shared_ptr<BSPNode>  prevNode;
  std::shared_ptr<BSPNode>  leftChunk;
  std::shared_ptr<BSPNode>  rightChunk;
  std::shared_ptr<BSPNode>  frontChunk;
  std::shared_ptr<BSPNode>  backChunk;
  std::shared_ptr<BSPNode>  topChunk;
  std::shared_ptr<BSPNode>  bottomChunk;
  bool inUse;
  bool toRender;
  bool toBuild;
  bool toDelete;
};

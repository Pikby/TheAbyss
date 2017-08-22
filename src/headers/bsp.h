#include <list>
#include <memory>
#include "perlinnoise.h"
#include "world.h"
//Class which holds the data for each individual chunk
class BSPNode;
class BSP
{
private:
  Shader* blockShader;
  const char* texture;
  Block** dictionary;
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
  char worldMap[CHUNKSIZE*CHUNKSIZE*CHUNKSIZE];
  GLuint VBO, EBO, VAO;
  GLuint depthMapFBO;
  GLuint* glTexture;
  int addVertex(float x, float y, float z,float xn, float yn, float zn, float texX, float texY);
  void addIndices(int index1, int index2, int index3, int index4);
public:

  long int xCoord;
  long int yCoord;
  long int zCoord;
  BSP(Shader* shader, Block** dict, GLuint* newglTexture, long int x, long int y, long int z,  siv::PerlinNoise* perlin);
  BSP();
  ~BSP();
  void generateTerrain(  siv::PerlinNoise* perlin);
  void render();
  void addBlock(int x, int y, int z,int id);
  void freeGL();
  bool blockExists(int x,int y,int z);
  int getBlock(int x, int y, int z);
  int removeBlock(int x, int y, int z);
  void build(World* curWorld,std::shared_ptr<BSPNode>  curRightChunk,std::shared_ptr<BSPNode>  curLeftChunk,std::shared_ptr<BSPNode>  curTopChunk,
                       std::shared_ptr<BSPNode>  curBottomChunk,std::shared_ptr<BSPNode>  curFrontChunk,std::shared_ptr<BSPNode>  curBackChunk);
  void draw(Camera* camera,float lightposx,float lightposy,float lightposz);

};

class BSPNode
{
  public:
  BSP curBSP;
  BSPNode(Shader* shader, Block** dict, GLuint* newglTexture, long int x, long int y, long int z,  siv::PerlinNoise* perlin);
  ~BSPNode();
  bool blockExists(int x, int y, int z);
  void build(World* curWorld);
  void draw(Camera* camera,float lightposx,float lightposy,float lightposz);
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

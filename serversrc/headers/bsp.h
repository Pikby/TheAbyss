#include <list>
#include <memory>
#include "perlinnoise.h"
#include "world.h"
#include "items.h"

enum renderType {OPAQUE,TRANSLUCENT,TRANSPARENT};
//Class which holds the data for each individual chunk
class BSP
{
private:
  unsigned char worldMap[CHUNKSIZE*CHUNKSIZE*CHUNKSIZE];
  std::string worldName;
public:
  long int xCoord;
  long int yCoord;
  long int zCoord;

  BSP(int x, int y, int z,std::string worldName);
  BSP();
  ~BSP();
  void generateTerrain();
  void addBlock(int x, int y, int z,char id);
  bool blockExists(int x,int y,int z);
  int blockVisibleType(int x, int y, int z);
  int getBlock(int x, int y, int z);
  void delBlock(int x, int y, int z);
  void saveChunk();
  std::shared_ptr<std::string> compressChunk();
  glm::vec3 offset(float x, float y,float z);
  void build(std::shared_ptr<BSPNode>  curRightChunk,std::shared_ptr<BSPNode>  curLeftChunk,std::shared_ptr<BSPNode>  curTopChunk,
                       std::shared_ptr<BSPNode>  curBottomChunk,std::shared_ptr<BSPNode>  curFrontChunk,std::shared_ptr<BSPNode>  curBackChunk);
};

class BSPNode
{
  public:
  BSP curBSP;
  BSPNode(int x, int y, int z,std::string worldName);
  ~BSPNode();
  void saveChunk();
  bool blockExists(int x, int y, int z);
  int blockVisibleType(int x, int y, int z);
  void build();
  void generateTerrain();
  std::shared_ptr<std::string> getCompressedChunk();
  void delBlock(int x, int y, int z);
  void addBlock(int x, int y, int z, int id);
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
  bool toDelete;
  bool isGenerated;
};

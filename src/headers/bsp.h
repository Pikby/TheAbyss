#include "perlinnoise.h"

class World;
class BSP;
class Block;

//Class which holds the data for each individual chunk
class BSP
{
private:

  Shader* blockShader;
  const char* texture;
  std::vector<Block>* dictionary;
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;


  char worldMap[16*16*16];


  GLuint VBO, EBO, VAO;
  GLuint depthMapFBO;
  GLuint* glTexture;


  int addVertex(float x, float y, float z,float xn, float yn, float zn, float texX, float texY);
  void addIndices(int index1, int index2, int index3, int index4);

public:
  BSP* leftChunk;
  BSP* rightChunk;
  BSP* frontChunk;
  BSP* backChunk;
  BSP* topChunk;
  BSP* bottomChunk;

  bool isDrawing;
  bool isBuilt;
  bool isRendered;
  long int xCoord;
  long int zCoord;
  long int yCoord;
  BSP(Shader* shader, std::vector<Block> * dict, GLuint* newglTexture, long int x, long int y, long int z,  siv::PerlinNoise* perlin);
  BSP();
  ~BSP();
  bool addBlock(int x, int y, int z,int id);
  bool blockExists(int x,int y,int z);
  int getBlock(int x, int y, int z);
  int removeBlock(int x, int y, int z);
  void build(World* curWorld);
  void render();
  void draw(Camera* camera,float lightposx,float lightposy,float lightposz);
  void generateTerrain(  siv::PerlinNoise* perlin);
};


//Class which encapsulates all the chunks as well as the shaders and dicionary
class World
{
private:

  unsigned int totalChunks;
  GLuint glTexture;
  Shader* blockShader;
  siv::PerlinNoise* perlin;
  const char* texture;
  int horzRenderDistance;
  int vertRenderDistance;
  std::vector<Block> dictionary;
  float lightposx,lightposy,lightposz;
public:
  std::queue<BSP*> renderQueue;
  bool loadDictionary(const char* file);
  World();
  void renderWorld(int x, int y, int z);
  void drawWorld(int x, int y, int z, Camera* camera);
  bool chunkExists(int x, int y, int z);
  BSP* getChunk(int x, int y, int z);
  bool blockExists(int x, int y, int z);
  void delChunk(int x, int y, int z);
  void drawShadows();
  std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, BSP>>> BSPmap;
};



//The class for each individual block in the dictionary
class Block
{
public:
  int id;
  int texArray[12]; //array of coordinates of all sides of the block from the texture array
  int width;
  int height;
  int atlasWidth;
  int atlasHeight;

  Block(int newId, int* array, int newWidth,
    int newHeight,int newAtlasWidth, int newAtlasHeight);

  void getTop(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[0]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[1]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[0]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[1]+1);
  };

  void getBottom(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[2]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[3]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[2]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[3]+1);
  };

  void getLeft(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[4]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[5]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[4]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[5]+1);
  };

  void getRight(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[6]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[7]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[6]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[7]+1);
  };

  void getFront(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[8]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[9]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[8]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[9]+1);
  };

  void getBack(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[10]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[11]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[10]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[11]+1);
  };


};

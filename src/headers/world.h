 #define CHUNKSIZE 32
//Class which encapsulates all the chunks as well as the shaders and dicionary
class BSPNode;
class BSP;
class Block;
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
  Block** dictionary;
  float lightposx,lightposy,lightposz;
  GLuint VAO;
  void addToBuildQueue(std::shared_ptr<BSPNode> curNode);
public:
  int numbOfThreads;
  std::shared_ptr<BSPNode> frontNode;
  std::shared_ptr<BSPNode> frontDelNode;
  bool loadDictionary(const char* file);
  World(int numbBuildThreads);
  ~World();
  void renderWorld(float* mainx, float* mainy, float* mainz);
  void drawWorld(Camera* camera);
  void buildWorld(int threadNumb);
  bool chunkExists(int x, int y, int z);
  std::shared_ptr<BSPNode> getChunk(int x, int y, int z);
  bool blockExists(int x, int y, int z);
  void delChunk(int x, int y, int z);
  void delScan(float* mainx, float* mainy, float* mainz);
  void generateChunk(int chunkx, int chunky, int chunkz);
  void drawShadows();
  std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::shared_ptr<BSPNode>>>> BSPmap;
  std::queue<std::shared_ptr<BSPNode>>* buildQueue;

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

  Block();
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

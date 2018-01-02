
#define CHUNKSIZE 32
//Class which encapsulates all the chunks as well as the shaders and dicionary
class BSPNode;
class BSP;
class Block;
class WorldWrap
{
protected:
  //

  static int screenWidth;
  static int screenHeight;
  static unsigned int totalChunks;
  //global texture atlas

  //Render distances
  static int horzRenderDistance;
  static int vertRenderDistance;

  static int renderBuffer;
  //The seed of the world, for generation
  static int seed;
  //Array of blocks that make up the dictionary
  static Block* dictionary;
  //Perlin noise object
  static FastNoise perlin;
  //Number of build threads
  static int numbOfThreads;
  //The name of the world, for saving and loading
  static std::string worldName;
  static bool blockExists(int x, int y, int z);
  static bool blockExists(glm::vec3 pos)
  {
    return blockExists((int)floor(pos.x),(int)floor(pos.y),(int)floor(pos.z));
  }
  static bool blockExists(double x,double y, double z)
  {
    return blockExists((int)floor(x),(int)floor(y),(int)floor(z));
  }
  static int anyExists(glm::vec3 pos);
  static bool entityExists(glm::vec3 pos);
  static bool entityExists(float x, float y, float z);

  static std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::shared_ptr<BSPNode>>>> BSPmap;
  static std::shared_ptr<BSPNode> getChunk(int x, int y, int z);
  enum target{BLOCK = 0, NOTHING = -1};
  static glm::vec4 rayCast(glm::vec3 pos, glm::vec3 front, int max);
};

class World : public WorldWrap
{
  private:
    GLuint glTexture;
    const char* texture;
    std::shared_ptr<BSPNode> frontNode;
    std::shared_ptr<BSPNode> frontDelNode;
  public:
     unsigned int depthMapFBO,depthMap,depthMapEBO;
     unsigned int quadVAO = 0;
     unsigned int quadVBO;
     void addToBuildQueue(std::shared_ptr<BSPNode> curNode);
     bool loadDictionary(const char* file);
     World(int numbBuildThreads,int width,int height);

     void renderWorld(float* mainx, float* mainy, float* mainz);
     void drawWorld();
     void buildWorld(int threadNumb);
     bool chunkExists(int x, int y, int z);

     void setLightLocation(glm::vec3 pos)
     {
     }
     void delBlock(int x, int y, int z);
     void delChunk(int x, int y, int z);
     void addBlock(int x, int y, int z, int id);
     void updateBlock(int x, int y, int z);
     void delScan(float* mainx, float* mainy, float* mainz);
     void generateChunk(int chunkx, int chunky, int chunkz);
     void drawTranslucent();
     void drawOpaque();
     void saveWorld();
     void renderQuad();
     std::queue<std::shared_ptr<BSPNode>>* buildQueue;
};

//The class for each individual block in the dictionary
#define BLOCKRES 128
class Block
{
public:
  std::string name;
  int id;
  int texArray[12]; //array of coordinates of all sides of the block from the texture array
  int width;
  int height;
  int depth;
  int atlasWidth;
  int atlasHeight;
  int visibleType;

  Block(std::string newName, int newId, int* array,int newVisibleType, int newWidth,
    int newHeight, int newDepth,int newAtlasWidth, int newAtlasHeight);


  void print()
  {
    std::cout << name << "\n"
              << id << "\n"
              << visibleType << "\n";
  }
  Block(){};
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

  bool isInBlock(glm::vec3 pos)
  {
    if(pos.x>0 && pos.x<(float)width/BLOCKRES)
      if(pos.y>0 && pos.y<(float)height/BLOCKRES)
        if(pos.z>0 && pos.z<(float)depth/BLOCKRES)
        {
          return true;
        }

    return false;
  };

};

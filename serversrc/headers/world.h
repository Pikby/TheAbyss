
#define CHUNKSIZE 32
//Class which encapsulates all the chunks as well as the shaders and dicionary
class BSPNode;
class BSP;
class Block;
class MainChar;
class WorldWrap
{
protected:
  static unsigned int totalChunks;
  //global texture atlas
  //The seed of the world, for generation
  static int seed;
  //Perlin noise object
  static FastNoise perlin;
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

public:
};

class World : public WorldWrap
{
  private:
    std::shared_ptr<BSPNode> frontNode;
    std::shared_ptr<BSPNode> frontDelNode;
  public:
     void addToBuildQueue(std::shared_ptr<BSPNode> curNode);
     World();

     bool chunkExists(int x, int y, int z);
     void delBlock(int x, int y, int z);
     void delChunk(int x, int y, int z);
     void addBlock(int x, int y, int z, int id);
     void updateBlock(int x, int y, int z);
     void delScan(float* mainx, float* mainy, float* mainz);
     void generateChunk(int chunkx, int chunky, int chunkz);
     void saveWorld();
};

//The class for each individual block in the dictionary

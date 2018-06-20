
#pragma once
typedef unsigned char uchar;
typedef unsigned int uint;
#define CHUNKSIZE 32

#include <memory>
#include <mutex>
#include <map>
#include <queue>
#include <glm/glm.hpp>
#include "../headers/objects.h"
#include "../MainEngine/messenger.h"
#include "../headers/threadSafeQueue.h"
struct PointLight
{
  glm::vec3 position;

  glm::vec3 ambient;
  glm::vec3 specular;
  glm::vec3 diffuse;

  float constant;
  float linear;


  float quadratic;

  glm::mat4 shadowTransform[6];
  unsigned int depthMapFBO,depthCubemap;
};
struct DirLight
{
  glm::vec3 direction;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  glm::mat4 lightSpaceMat;
  unsigned int depthMapFBO,depthMap;

};


#include "../headers/3dmap.h"


//Class which encapsulates all the chunks as well as the shaders and dicionary
class BSPNode;
class BSP;
class Block;
class MainChar;
class Player;
enum target{BLOCK = 0, NOTHING = -1};
class World
{
  private:
    uint glTexture;
    const char* texture;
    std::shared_ptr<BSPNode> frontNode;
    std::shared_ptr<BSPNode> frontDelNode;
    std::vector<std::shared_ptr<Object>> objList;
    std::vector<PointLight> lightList;

    DirLight dirLight;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f),16.0f/9.0f,1.0f,25.0f);
    glm::mat4 viewProj;
    glm::mat4 hsrMat;
    glm::mat4 viewMat;
    glm::vec3 viewPos;

    Shader objShader,dirDepthShader,pointDepthShader,blockShader;
    Shader debugDepthQuad;
    public:
    Messenger messenger;

    void drawFinal();
    void renderDirectionalDepthMap();
    void renderPointDepthMap(int id);
    void startPointShadowDraw(Shader* shader, int id);
    void renderDirectionalShadows();
    void bindDirectionalShadows(Shader* shader);
    void renderPointShadows();
    void bindPointShadows();
    void setLights(Shader* shader);
    void setupSockets(std::string ipAddress);
    void addCube(Cube newCube);
    std::shared_ptr<Object> getObject(int id) {return objList[id];}
    void drawObjects();
    void updateViewMatrix(glm::mat4 viewMatrix,glm::mat4 hsrMatrix,glm::vec3 viewPosition)
    {
      viewMat = viewMatrix;
      hsrMat = hsrMatrix;
      viewPos = viewPosition;
    }
    void addLight(glm::vec3 pos,
             glm::vec3 amb = glm::vec3(0.2f,0.2f,0.2f),
             glm::vec3 spe = glm::vec3(1.0f,1.0f,1.0f),
             glm::vec3 dif = glm::vec3(0.5f,0.5f,0.5f),
             float cons = 1.0f, float lin = 0.14f, float quad = 0.07f)
    {
      lightList.push_back(PointLight{pos,amb,spe,dif,cons,lin,quad});
      renderPointDepthMap(lightList.size()-1);
    }

    void createDirectionalLight(glm::vec3 dir = glm::vec3(-1.0f,-1.0f,-1.0f),
                                glm::vec3 amb = glm::vec3(0.8f,0.8f,0.8f),
                                glm::vec3 dif = glm::vec3(0.5f,0.5f,0.5f),
                                glm::vec3 spec = glm::vec3(1.0f,1.0f,1.0f))
    {
      dirLight = {dir,amb,dif,spec};
      renderDirectionalDepthMap();
    }



    int screenWidth,screenHeight;
    unsigned int totalChunks;
    //global texture atlas

    //Render distances
    int horzRenderDistance,vertRenderDistance,renderBuffer;
    //The seed of the world, for generation
    int drawnChunks;

    //Number of build threads
    int numbOfThreads;
    //The name of the world, for saving and loading
    std::string worldName;
    bool blockExists(int x, int y, int z);
    bool blockExists(glm::vec3 pos)
    {
      return blockExists((int)floor(pos.x),(int)floor(pos.y),(int)floor(pos.z));
    }
    bool blockExists(double x,double y, double z)
    {
      return blockExists((int)floor(x),(int)floor(y),(int)floor(z));
    }
    int anyExists(glm::vec3 pos);
    bool entityExists(glm::vec3 pos);
    bool entityExists(float x, float y, float z);

    Map3D<std::shared_ptr<BSPNode>> BSPmap;

    std::shared_ptr<BSPNode> getChunk(int x, int y, int z);

    glm::vec4 rayCast(glm::vec3 pos, glm::vec3 front, int max);
    std::mutex playerListMutex;
    std::map<uchar, std::shared_ptr<Player>> playerList;
    unsigned int quadVAO = 0;
    unsigned int quadVBO;

    char mainId;
    void addToBuildQueue(std::shared_ptr<BSPNode> curNode);
    World(int numbBuildThreads,int width,int height);

    void renderWorld(float* mainx, float* mainy, float* mainz);
    //void drawWorld(glm::mat4 viewMat, glm::mat4 projMat, bool useHSR);
    void drawTerrain(Shader* shader = NULL, bool useHSR = false);
    void drawPlayers(Shader* shader);
    void buildWorld(int threadNumb);
    bool chunkExists(int x, int y, int z);
    void addPlayer(float x, float y, float z, uchar id);
    void removePlayer(uchar id);
    void movePlayer(float x,float y, float z, uchar id);

    inline void sendMessage(int* buf,int length);
    void setLightLocation(glm::vec3 pos)
    {
    }
    void delBlock(int x, int y, int z);
    void delChunk(int x, int y, int z);
    void addBlock(int x, int y, int z, int id);
    void updateBlock(int x, int y, int z);
    void delScan(float* mainx, float* mainy, float* mainz);
    void generateChunk(int chunkx, int chunky, int chunkz);
    void generateChunkFromString(int chunkx, int chunky, int chunkz,const std::string &val);
    void saveWorld();
    void loadChunk(std::string);
    std::queue<std::shared_ptr<BSPNode>>* buildQueue;
};

//The class for each individual block in the dictionary

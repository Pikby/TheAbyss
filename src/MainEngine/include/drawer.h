#define NUMBOFCASCADEDSHADOWS 1

#include <list>
#include <map>

#include "../../Objects/include/skybox.h"



class BSPNode;
class Camera;
struct PointLight
{
  glm::vec3 position;

  glm::vec3 ambient;
  glm::vec3 specular;
  glm::vec3 diffuse;


  float radius;
  float constant;
  float linear;
  float quadratic;

  glm::mat4 shadowTransform[6];
  uint depthMapFBO;
  uint depthCubemap;
};

struct DirLight
{
  glm::vec3 direction;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  glm::mat4 lightSpaceMat[NUMBOFCASCADEDSHADOWS];
  uint depthMapFBO[NUMBOFCASCADEDSHADOWS];
  uint depthMap[NUMBOFCASCADEDSHADOWS];
  float arrayOfDistances[NUMBOFCASCADEDSHADOWS];
};

class Drawer
{
private:
  //Flags
  bool drawChunkOutlines = false;




  SkyBox skyBox;
  uint textureAtlas;

  int screenWidth,screenHeight;
  std::vector<std::shared_ptr<Object>> objList;
  std::vector<PointLight> lightList;

  DirLight dirLight;
  uint gBuffer,gDepth;
  uint gPosition, gNormal, gColorSpec;
  uint transBuffer, transTexture, transDepth;
  //uint PPBuffer,PPTexture,PPTextureBright;
  //uint pingPongFBO[2],pingPongTexture[2];
  //std::unique_ptr<Shader> PPShader;
  std::unique_ptr<Shader> blockShader;
  std::unique_ptr<Shader> transShader,depthBufferLoadingShader;

  //void calculateFrustrum(glm::vec3* arr,const glm::vec3 &pos,const glm::vec3 &front,const glm::vec3 &right,const glm::vec3 &up, float camZoomInDegrees,float ar,float near, float far);
  void renderQuad();
  void createTextureAtlas(const char* texture, int cellWidth);
  glm::ivec3 toChunkCoords(const glm::ivec3& in);
  void genMultisampleTexture(uint* name, int samples,GLenum attachment);
public:
  Drawer();

  void setDrawChunkOutlines(bool b){drawChunkOutlines = b;}
  void calculateMinandMaxPoints(const glm::vec3* array, int arrsize, glm::vec3* finmin,glm::vec3* finmax);
  glm::ivec2 textureAtlasDimensions;
  uint getTextureAtlasID(){return textureAtlas;}

  std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> chunksToDraw;
  std::map<uint8_t, std::shared_ptr<Player>> playerList;



  void setExposure(float exp);
  void updateViewProjection(float camZoom,float near=0, float far=0);
  void setupShadersAndTextures(int screenWidth, int screenHeight);
  void setRenderDistances(int vert,int horz,int buffer);
  void renderDirectionalDepthMap();
  void renderPointDepthMap(int id);
  void startPointShadowDraw(Shader* shader, int id);
  void renderDirectionalShadows();
  void bindDirectionalShadows(Shader* shader);
  void deleteAllBuffers();
  void setAllBuffers();
  void setTerrainColor(const glm::vec3 &color);

  void updateShadows(bool val);
  void renderPointShadows();
  void renderGBuffer();
  void bindPointShadows();
  void setLights(Shader* shader);
  void addCube(Cube newCube);
  std::shared_ptr<Object> getObject(int id) {return objList[id];}
  void drawObjects();
  void drawPlayers();
  void drawFinal();
  void addLight(glm::vec3 pos, glm::vec3 amb = glm::vec3(1.0f,1.0f,1.0f),
                glm::vec3 spe = glm::vec3(1.0f,1.0f,1.0f), glm::vec3 dif = glm::vec3(0.5f,0.5f,0.5f),
                float cons = 1.0f, float lin = 0.14f, float quad = 0.07f);
  void updateDirectionalLight(glm::vec3 dir = glm::vec3(-1.0f,-1.0f,-1.0f),glm::vec3 amb = glm::vec3(0.2f),
                              glm::vec3 dif = glm::vec3(0.5f),glm::vec3 spec = glm::vec3(1.0f));

  void drawTerrainOpaque(std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> list);
  void drawTerrainTranslucent(std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> list);
  void drawPlayers(Shader* shader);


};

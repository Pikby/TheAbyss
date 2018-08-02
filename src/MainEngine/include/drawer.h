#include "bsp.h"
#include <list>
#include <map>
#include "../../Objects/include/objects.h"
#include "../../headers/camera.h"


#define NUMBOFCASCADEDSHADOWS 4
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
  uint glTexture;
  const char* texture;

  int vertRenderDistance, horzRenderDistance,renderBuffer;

  //std::list<std::shared_ptr<BSPNode> chunksToDraw;
  std::vector<std::shared_ptr<Object>> objList;
  std::vector<PointLight> lightList;

  DirLight dirLight;
  glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f),16.0f/9.0f,1.0f,25.0f);
  glm::mat4 viewProj;
  glm::mat4 hsrMat;
  glm::mat4 hsrProj;
  glm::mat4 viewMat;


  glm::vec3 viewPos;
  glm::vec3 viewFront;
  glm::vec3 viewUp;
  glm::vec3 viewRight;

  //Plane goes bottomleft,topleft,topright,bottomright;
  //0-3 is near 4-7 is far
  glm::vec3 viewFrustrum[8];




  float camZoomInDegrees;
  float camNear;
  float camFar;

  Shader objShader,dirDepthShader,pointDepthShader,blockShader;
  Shader debugDepthQuad;
  void calculateFrustrum(glm::vec3* arr, float near, float far);
  void calculateMinandMaxPoints(glm::vec3* array, int arrsize, glm::vec3* finmin,glm::vec3* finmax);
public:
  std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> chunksToDraw;
  std::map<uchar, std::shared_ptr<Player>> playerList;
  glm::vec3 viewMin;
  glm::vec3 viewMax;
  float directionalShadowResolution;
  int screenWidth,screenHeight;
  void updateViewProjection(float camZoom,float near=0, float far=0);
  void setupShadersAndTextures(int screenWidth, int screenHeight);
  void setRenderDistances(int vert,int horz,int buffer);
  void renderDirectionalDepthMap();
  void renderPointDepthMap(int id);
  void startPointShadowDraw(Shader* shader, int id);
  void renderDirectionalShadows();
  void bindDirectionalShadows(Shader* shader);

  void renderPointShadows();
  void bindPointShadows();
  void setLights(Shader* shader);
  void addCube(Cube newCube);
  std::shared_ptr<Object> getObject(int id) {return objList[id];}
  void drawObjects();
  void drawPlayers();
  void drawFinal();
  void updateCameraMatrices(Camera* cam);
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
                              glm::vec3 amb = glm::vec3(0.5f,0.5f,0.5f),
                              glm::vec3 dif = glm::vec3(0.5f,0.5f,0.5f),
                              glm::vec3 spec = glm::vec3(1.0f,1.0f,1.0f))
  {
    dirLight = {dir,amb,dif,spec};
    renderDirectionalDepthMap();
  }
  void drawTerrain(Shader* shader,  std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> list);
  void drawPlayers(Shader* shader);



};

#pragma once

#include "bsp.h"
#include <list>
#include <map>
#include "../../Objects/include/objects.h"
#include "../../headers/camera.h"
#include "../../headers/3dmap.h"

#define NUMBOFCASCADEDSHADOWS 2
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
  SkyBox skyBox;
  uint textureAtlas;


  float MSAA = 2;
  int vertRenderDistance, horzRenderDistance,renderBuffer;
  std::vector<std::shared_ptr<Object>> objList;
  std::vector<PointLight> lightList;

  DirLight dirLight;
  glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f),16.0f/9.0f,1.0f,25.0f);

  //The camera matrices, extracted from the camera each frame
  glm::mat4 viewProj,viewMat;

  //The camera vectors, extracted from the camera each frame,
  glm::vec3 viewPos, viewFront,viewUp,viewRight;

  float camZoomInDegrees,camNear, camFar;
  uint gBuffer,gDepth;
  uint gPosition, gNormal, gColorSpec;
  uint transBuffer, transTexture, transDepth;;
  Shader objShader,dirDepthShader,pointDepthShader,blockShader,gBufferShader;
  Shader transShader;
  Shader quadShader;
  Shader depthBufferLoadingShader;
  static void calculateFrustrum(glm::vec3* arr,const glm::vec3 &pos,const glm::vec3 &front,const glm::vec3 &right,const glm::vec3 &up, float camZoomInDegrees,float ar,float near, float far);
  void calculateMinandMaxPoints(const glm::vec3* array, int arrsize, glm::vec3* finmin,glm::vec3* finmax);
  void renderQuad();
public:
  glm::ivec2 textureAtlasDimensions;
  uint getTextureAtlasID(){return textureAtlas;}
  //Plane goes bottomleft,topleft,topright,bottomright;
  //0-3 is near 4-7 is far
  glm::vec3 viewFrustrum[8];
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
  void deleteAllBuffers();
  void setAllBuffers();
  void setTerrainColor(const glm::vec3 &color);

  void updateShadows(bool val) {blockShader.use();blockShader.setBool("shadowsOn",val);}
  void updateAntiAliasing(float MSAA);
  void renderPointShadows();
  void renderGBuffer();
  void bindPointShadows();
  void setLights(Shader* shader);
  void addCube(Cube newCube);
  std::shared_ptr<Object> getObject(int id) {return objList[id];}
  void drawObjects();
  void drawPlayers();
  void drawFinal();
  void updateCameraMatrices(Camera* cam);
  void addLight(glm::vec3 pos, glm::vec3 amb = glm::vec3(0.0f,0.0f,0.0f),
                glm::vec3 spe = glm::vec3(1.0f,1.0f,1.0f), glm::vec3 dif = glm::vec3(0.5f,0.5f,0.5f),
                float cons = 1.0f, float lin = 0.14f, float quad = 0.07f);
  void updateDirectionalLight(glm::vec3 dir = glm::vec3(-1.0f,-1.0f,-1.0f),glm::vec3 amb = glm::vec3(0.5f,0.5f,0.5f),
                              glm::vec3 dif = glm::vec3(0.5f,0.5f,0.5f),glm::vec3 spec = glm::vec3(1.0f,1.0f,1.0f));

  void drawTerrainOpaque(Shader* shader,  std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> list);
  void drawTerrainTranslucent(Shader* shader,  std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> list);
  void drawPlayers(Shader* shader);



};

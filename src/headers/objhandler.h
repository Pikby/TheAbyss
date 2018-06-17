#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "objects.h"
#include "shaders.h"

#define SCR_WIDTH 1024
#define SCR_HEIGHT 720
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

class EntityWorld
{
  private:
  std::vector<std::shared_ptr<Object>> objList;
  std::vector<PointLight> lightList;

  DirLight dirLight;
  glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f),16.0f/9.0f,1.0f,25.0f);

  glm::mat4 viewMat;
  glm::vec3 viewPos;

  Shader objShader,dirDepthShader,pointDepthShader;
  void renderDirectionalDepthMap();
  void renderPointDepthMap(int id);

  void startPointShadowDraw(Shader* shader, int id);

public:
  World();
  void renderDirectionalShadows();
  void bindDirectionalShadows();
  void renderPointShadows();
  void bindPointShadows();
  void setLights();
  void addCube(Cube newCube);
  std::shared_ptr<Object> getObject(int id) {return objList[id];}
  void drawObjects();
  void updateViewMatrix(glm::mat4 viewMatrix,glm::vec3 viewPosition)
  {
    viewMat = viewMatrix;
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
                              glm::vec3 amb = glm::vec3(0.2f,0.2f,0.2f),
                              glm::vec3 dif = glm::vec3(0.5f,0.5f,0.5f),
                              glm::vec3 spec = glm::vec3(1.0f,1.0f,1.0f))
  {
    dirLight = {dir,amb,dif,spec};
    renderDirectionalDepthMap();
  }



};

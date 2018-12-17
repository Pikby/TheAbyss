#include "headers/objhandler.h"

World::World()
{
  objShader        = Shader("../src/shaders/entShader.vs",
                            "../src/shaders/entShader.fs");

  objShader.use();
  objShader.setInt("dirLight.shadow",4);
  objShader.setInt("pointLights[0].shadow",5);
  objShader.setInt("pointLights[1].shadow",6);
  objShader.setFloat("far_plane",25.0f);
  glm::mat4 projectMat = glm::perspective(glm::radians(45.0f),
                  (float)1920/ (float)1080, 0.1f, 100.0f);
  objShader.setMat4("projection",projectMat);

  dirDepthShader   = Shader("../src/shaders/dirDepthShader.fs",
                            "../src/shaders/dirDepthShader.vs");

  pointDepthShader = Shader("../src/shaders/pointDepthShader.fs",
                            "../src/shaders/pointDepthShader.vs",
                            "../src/shaders/pointDepthShader.gs");

}

void World::addCube(Cube newCube)
{
  objList.push_back(std::make_shared<Cube>(newCube));
}

void World::renderDirectionalDepthMap()
{
  float near_plane = 1.0f, far_plane = 40.0f;
  glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  glm::mat4 lightView = glm::lookAt(glm::vec3( 10.0f, 30.0f, 10.0f),
                                    glm::vec3( 0.0f, 0.0f,  0.0f),
                                    glm::vec3( 0.0f, 1.0f,  0.0f));
  dirLight.lightSpaceMat = lightProjection * lightView;

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
  glGenFramebuffers(1, &dirLight.depthMapFBO);
  glGenTextures(1, &dirLight.depthMap);

  glBindTexture(GL_TEXTURE_2D, dirLight.depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
               SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


  glBindFramebuffer(GL_FRAMEBUFFER, dirLight.depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dirLight.depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void World::renderPointDepthMap(int id)
{
  PointLight* light = &(lightList[id]);
  glGenFramebuffers(1,&(light->depthMapFBO));
  glGenTextures(1,&(light->depthCubemap));
  glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthCubemap);

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

  for(int i =0;i<6;i++)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER, light->depthMapFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light->depthCubemap,0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  glm::vec3 pos = light->position;
  glm::mat4 proj= glm::perspective(glm::radians(90.0f),1.0f,1.0f,25.0f);
  light->shadowTransform[0] = proj*glm::lookAt(pos, pos + glm::vec3(1.0,0.0,0.0),glm::vec3(0.0,-1.0,0.0));
  light->shadowTransform[1] = proj*glm::lookAt(pos, pos + glm::vec3(-1.0,0.0,0.0),glm::vec3(0.0,-1.0,0.0));
  light->shadowTransform[2] = proj*glm::lookAt(pos, pos + glm::vec3(0.0,1.0,0.0),glm::vec3(0.0,0.0,1.0));
  light->shadowTransform[3] = proj*glm::lookAt(pos, pos + glm::vec3(0.0,-1.0,0.0),glm::vec3(0.0,0.0,-1.0));
  light->shadowTransform[4] = proj*glm::lookAt(pos, pos + glm::vec3(0.0,0.0,1.0),glm::vec3(0.0,-1.0,0.0));
  light->shadowTransform[5] = proj*glm::lookAt(pos, pos + glm::vec3(0.0,0.0,-1.0),glm::vec3(0.0,-1.0,0.0));

}


void World::setLights()
{

  objShader.setVec3("dirLight.direction", dirLight.direction);
  objShader.setVec3("dirLight.ambient", dirLight.ambient);
  objShader.setVec3("dirLight.diffuse", dirLight.diffuse);
  objShader.setVec3("dirLight.specular", dirLight.specular);


  unsigned int numbOfLights = lightList.size();
  objShader.setInt("numbOfLights",numbOfLights);
  for(uint i=0;i<numbOfLights;i++)
  {
    PointLight curLight = lightList[i];
    objShader.setVec3("pointLights[" + std::to_string(i) + "].position",curLight.position);
    objShader.setVec3("pointLights[" + std::to_string(i) + "].ambient",curLight.ambient);
    objShader.setVec3("pointLights[" + std::to_string(i) + "].specular",curLight.specular);
    objShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse",curLight.diffuse);
    objShader.setFloat("pointLights[" + std::to_string(i) + "].constant",curLight.constant);
    objShader.setFloat("pointLights[" + std::to_string(i) + "].linear",curLight.linear);
    objShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic",curLight.quadratic);
  }
}


void World::renderDirectionalShadows()
{
  dirDepthShader.use();

  dirDepthShader.setMat4("lightSpace",dirLight.lightSpaceMat);
  glViewport(0,0,1024,1024);
  glBindFramebuffer(GL_FRAMEBUFFER, dirLight.depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    for(int i =0;i<objList.size();i++)
    {
      objList[i]->draw(&dirDepthShader);
    }
  glBindFramebuffer(GL_FRAMEBUFFER,0);

}
void World::bindDirectionalShadows()
{
  glActiveTexture(GL_TEXTURE4);

  objShader.use();
  glBindTexture(GL_TEXTURE_2D, dirLight.depthMap);
  objShader.setMat4("lightSpace",dirLight.lightSpaceMat);
}

void World::renderPointShadows()
{
  pointDepthShader.use();

  for(int i=0;i<objList.size();i++)
  {
    PointLight* light = &(lightList[i]);
    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER,light->depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      pointDepthShader.setVec3("lightPos",light->position);
      pointDepthShader.setFloat("far_plane", 25.0f);

      for(int i=0;i<6;i++)
      {
        pointDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", light->shadowTransform[i]);
      }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
  }
}

void World::bindPointShadows()
{
  objShader.use();
  unsigned int numbOfLights = lightList.size();
  for(uint i=0;i<numbOfLights;i++)
  {
    PointLight* curLight= &(lightList[i]);
    glActiveTexture(GL_TEXTURE5+i);
    glBindTexture(GL_TEXTURE_CUBE_MAP,curLight->depthCubemap);
  }
}

void World::drawObjects()
{
  glViewport(0,0,1280,720);
  objShader.use();

  objShader.setMat4("view",viewMat);
  objShader.setVec3("viewPos",viewPos);
  for(int i=0;i<objList.size();i++)
  {
    objList[i]->draw(&objShader);
  }
}

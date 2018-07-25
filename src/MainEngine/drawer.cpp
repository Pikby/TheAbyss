#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <limits>
#include "include/drawer.h"
#include "../TextureLoading/textureloading.h"

void Drawer::setupShadersAndTextures(int width, int height)
{
  const char* texture = "../assets/textures/atlas.png";
  glGenTextures(1, &glTexture);
  glBindTexture(GL_TEXTURE_2D, glTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  //Load and bind the texture from the class
  int texWidth, texHeight, nrChannels;
  unsigned char* image = loadTexture(texture, &texWidth,&texHeight,&nrChannels);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,0,GL_RGBA, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  freeTexture(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  screenWidth = width;
  screenHeight = height;

  objShader        = Shader("../src/Shaders/entShader.vs",
                            "../src/Shaders/entShader.fs");
  objShader.use();
  objShader.setInt("curTexture",0);
  objShader.setInt("dirLight.shadow",4);
  objShader.setInt("pointLights[0].shadow",5);
  objShader.setInt("pointLights[1].shadow",6);
  objShader.setFloat("far_plane",25.0f);

  blockShader = Shader("../src/Shaders/shaderBSP.vs","../src/Shaders/shaderBSP.fs");
  blockShader.use();
  blockShader.setInt("curTexture",0);
  blockShader.setInt("dirLight.shadow[0]",4);
  blockShader.setInt("dirLight.shadow[1]",5);
  blockShader.setInt("dirLight.shadow[2]",6);
  blockShader.setInt("dirLight.shadow[3]",7);
  blockShader.setFloat("far_plane",25.0f);

  blockShader.setInt("textureAtlasWidth",384);
  blockShader.setInt("textureAtlasHeight",128);
  blockShader.setInt("cellWidth",128);

  debugDepthQuad = Shader("../src/Shaders/old/debugQuad.vs", "../src/Shaders/old/debugQuad.fs");
  debugDepthQuad.use();
  debugDepthQuad.setInt("depthMap", 0);
  debugDepthQuad.setFloat("near_plane", -1.0f);
  debugDepthQuad.setFloat("far_plane", (vertRenderDistance+renderBuffer+1)*CHUNKSIZE*2);

  dirDepthShader   = Shader("../src/Shaders/dirDepthShader.fs",
                            "../src/Shaders/dirDepthShader.vs");
  dirDepthShader.use();
  glm::mat4 model;
  dirDepthShader.setMat4("model",model);
  pointDepthShader = Shader("../src/Shaders/pointDepthShader.fs",
                            "../src/Shaders/pointDepthShader.vs",
                            "../src/Shaders/pointDepthShader.gs");
}

void Drawer::setRenderDistances(int vert, int horz, int buffer)
{
  vertRenderDistance = vert;
  horzRenderDistance = horz;
  renderBuffer = buffer;
}
void Drawer::addCube(Cube newCube)
{
  objList.push_back(std::make_shared<Cube>(newCube));
}

void Drawer::renderDirectionalDepthMap()
{

  for(int i=0;i<NUMBOFCASCADEDSHADOWS;i++)
  {
    glGenFramebuffers(1, &(dirLight.depthMapFBO[i]));
    glGenTextures(1, &(dirLight.depthMap[i]));

    glBindTexture(GL_TEXTURE_2D, dirLight.depthMap[i]);
    int factor = i == 0 ? factor = 1 : factor = i*2;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 directionalShadowResolution/factor,directionalShadowResolution/factor, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


    glBindFramebuffer(GL_FRAMEBUFFER, dirLight.depthMapFBO[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dirLight.depthMap[i], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }


}

void Drawer::renderPointDepthMap(int id)
{
  PointLight* light = &(lightList[id]);
  glGenFramebuffers(1,&(light->depthMapFBO));
  glGenTextures(1,&(light->depthCubemap));
  glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthCubemap);



  float SHADOW_RES = 1024;
  for(int i =0;i<6;i++)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_DEPTH_COMPONENT,
                 SHADOW_RES, SHADOW_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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


void Drawer::setLights(Shader* shader)
{
  shader->use();
  shader->setVec3("dirLight.direction", dirLight.direction);
  shader->setVec3("dirLight.ambient", dirLight.ambient);
  shader->setVec3("dirLight.diffuse", dirLight.diffuse);
  shader->setVec3("dirLight.specular", dirLight.specular);

  shader->setInt("numbOfCascadedShadows",NUMBOFCASCADEDSHADOWS);
  unsigned int numbOfLights = lightList.size();
  shader->setInt("numbOfLights",numbOfLights);
  for(uint i=0;i<numbOfLights;i++)
  {
    PointLight curLight = lightList[i];
    shader->setVec3("pointLights[" + std::to_string(i) + "].position",curLight.position);
    shader->setVec3("pointLights[" + std::to_string(i) + "].ambient",curLight.ambient);
    shader->setVec3("pointLights[" + std::to_string(i) + "].specular",curLight.specular);
    shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse",curLight.diffuse);
    shader->setFloat("pointLights[" + std::to_string(i) + "].constant",curLight.constant);
    shader->setFloat("pointLights[" + std::to_string(i) + "].linear",curLight.linear);
    shader->setFloat("pointLights[" + std::to_string(i) + "].quadratic",curLight.quadratic);
  }
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Drawer::renderDirectionalShadows()
{
  const int PI = 3.14159265;
  double sunAngle = 80;
  int distToSun = (vertRenderDistance+1)*CHUNKSIZE;
  //Makes sure the light is always at the correct angle above the player
  glm::mat4 lightProjection,lightView, lightSpaceMatrix;

  float shadowNear = -1;
  float shadowFar = distToSun*3;

  for(int x = 0;x<NUMBOFCASCADEDSHADOWS;x++)
  {
    glm::vec3 frustrum[8];
    float near = camNear + ((camFar-camNear)/NUMBOFCASCADEDSHADOWS)*x;
    float far = camNear + ((camFar-camNear)/NUMBOFCASCADEDSHADOWS)*(x+1);
    calculateFrustrum(frustrum,near,far);
    dirLight.arrayOfDistances[x] = far;
    glm::vec3 lightTarget,lightPos;
    for(int i=0;i<8;i++)
    {
      lightTarget+=frustrum[i];
    }
    lightTarget /= 8;
    objList[0]->setPosition(lightTarget);
    objList[0]->updateModelMat();
    lightPos = lightTarget - dirLight.direction*distToSun;
    lightView  = glm::lookAt(lightPos,
                                      lightTarget,
                                      glm::vec3(0.0f,1.0f,0.0f));
    for(int i=0;i<8;i++)
    {
      frustrum[i] = glm::vec3(lightView*glm::vec4(frustrum[i],1.0f));
    }
    glm::vec3 min,max;
    calculateMinandMaxPoints(frustrum,8,&min,&max);
    //std::cout << glm::to_string(min) << ":" << glm::to_string(max) << "\n";
    lightProjection = glm::ortho(min.x, max.x, min.y,max.y,shadowNear, shadowFar);

    dirLight.lightSpaceMat[x] = lightProjection * lightView;


    debugDepthQuad.use();
    debugDepthQuad.setFloat("near_plane", shadowNear);
    debugDepthQuad.setFloat("far_plane", shadowFar);

    dirDepthShader.use();
    dirDepthShader.setMat4("lightSpaceMatrix",dirLight.lightSpaceMat[x]);
    int factor = x == 0 ? factor = 1 : factor = x*2;
    glViewport(0,0,directionalShadowResolution/factor,directionalShadowResolution/factor);
      glBindFramebuffer(GL_FRAMEBUFFER, dirLight.depthMapFBO[x]);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        drawTerrain(&dirDepthShader,hsrMat,true);
      glBindFramebuffer(GL_FRAMEBUFFER,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glViewport(0,0,screenWidth,screenHeight);


}


void Drawer::bindDirectionalShadows(Shader* shader)
{


  shader->use();
  for(int i=0;i<NUMBOFCASCADEDSHADOWS;i++)
  {
    glActiveTexture(GL_TEXTURE4+i);
    glBindTexture(GL_TEXTURE_2D, dirLight.depthMap[i]);
    shader->setMat4("dirLight.lightSpaceMatrix[" +std::to_string(i) + "]",dirLight.lightSpaceMat[i]);
    shader->setFloat("dirLight.arrayOfDistances[" + std::to_string(i) + "]",dirLight.arrayOfDistances[i]);
  }

}

void Drawer::renderPointShadows()
{
  pointDepthShader.use();

  for(int i=0;i<objList.size();i++)
  {
    PointLight* light = &(lightList[i]);
    glViewport(0, 0, directionalShadowResolution, directionalShadowResolution);
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

void Drawer::bindPointShadows()
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

void Drawer::drawObjects()
{
  glViewport(0,0,screenWidth,screenHeight);
  objShader.use();

  glBindTexture(GL_TEXTURE_CUBE_MAP,0);
  glBindTexture(GL_TEXTURE_2D,0);
  objShader.setMat4("view",viewMat);
  objShader.setVec3("viewPos",viewPos);
  for(int i=0;i<objList.size();i++)
  {
    objList[i]->draw(&objShader);
  }

}


void Drawer::drawPlayers()
{
  glViewport(0,0,screenWidth,screenHeight);
  objShader.use();

  glBindTexture(GL_TEXTURE_CUBE_MAP,0);
  glBindTexture(GL_TEXTURE_2D,0);
  objShader.setMat4("view",viewMat);
  objShader.setVec3("viewPos",viewPos);

  for(auto it = playerList.begin(); it != playerList.end();it++)
  {
    it->second->draw(&objShader);
  }
}

void Drawer::drawFinal()
{
  debugDepthQuad.use();
  glViewport(0,0,200,200);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, dirLight.depthMap[0]);
  // /renderQuad();
  glViewport(0,0,screenWidth,screenHeight);
  Shader* shader = &blockShader;
  shader->use();
  setLights(shader);
  bindDirectionalShadows(shader);

  shader->setMat4("view", viewMat);
  shader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
  shader->setVec3("viewPos", viewPos);
  //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  drawTerrain(shader,hsrMat);

  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


}

void Drawer::updateViewProjection(float camZoom,float near,float far)
{

  camZoomInDegrees = camZoom;
  camNear = near == 0 ? camNear : near;
  camFar  = far  == 0 ? camFar  : far;
  viewProj = glm::perspective(glm::radians(camZoom),
                              (float)screenWidth/(float)screenHeight,
                              camNear,camFar);

  hsrProj = glm::perspective(glm::radians(camZoom+10.0f),float(screenWidth)/(float)screenHeight
                           ,camNear,camFar+CHUNKSIZE*3);
  objShader.use();
  objShader.setMat4("projection",viewProj);
  blockShader.use();
  blockShader.setMat4("projection",viewProj);
}

void Drawer::updateCameraMatrices(Camera* cam)
{

  viewMat = cam->getViewMatrix();
  hsrMat = hsrProj*cam->getHSRMatrix();
  viewPos = cam->getPosition();
  viewFront = cam->front;
  viewUp = cam->up;
  viewRight = cam->right;
  calculateFrustrum(viewFrustrum,camNear,camFar);
  calculateMinandMaxPoints(viewFrustrum,8,&viewMin,&viewMax);
}

void Drawer::calculateFrustrum(glm::vec3* arr,float near, float far)
{
  float ar = (float)screenWidth/(float)screenHeight;
  float buffer = 0;
  float fovH = glm::radians(((camZoomInDegrees+buffer)/2)*ar);
  float fovV = glm::radians((camZoomInDegrees+buffer)/2);
  glm::vec3 curPos = viewPos - viewFront*(CHUNKSIZE*2);
  glm::vec3 rightaxis = glm::rotate(viewFront,fovH,viewUp);
  glm::vec3 toprightaxis = glm::rotate(rightaxis,fovV,viewRight);
  glm::vec3 bottomrightaxis = glm::rotate(rightaxis,-fovV,viewRight);
  glm::vec3 leftaxis = glm::rotate(viewFront,-fovH,viewUp);
  glm::vec3 topleftaxis = glm::rotate(leftaxis,fovV,viewRight);
  glm::vec3 bottomleftaxis = glm::rotate(leftaxis,-fovV,viewRight);


  //std::cout << "nnear and far" << near << ":" << far <<  "\n";
  float d;
  float a = near;
  d = a/(glm::dot(bottomleftaxis,viewFront));
  arr[0] = d*bottomleftaxis+curPos;
  d = a/(glm::dot(topleftaxis,viewFront));
  arr[1] = d*topleftaxis+curPos;
  d = a/(glm::dot(toprightaxis,viewFront));
  arr[2] =  d*toprightaxis+curPos;
  d = a/(glm::dot(bottomrightaxis,viewFront));
  arr[3] = d*bottomrightaxis+curPos;


  a = far;
  d = a/(glm::dot(bottomleftaxis,viewFront));
  arr[4] = d*bottomleftaxis+curPos;
  d = a/(glm::dot(topleftaxis,viewFront));
  arr[5] = d*topleftaxis+curPos;
  d = a/(glm::dot(toprightaxis,viewFront));
  arr[6] =  d*toprightaxis+curPos;
  d = a/(glm::dot(bottomrightaxis,viewFront));
  arr[7] = d*bottomrightaxis+curPos;


}

//Takes array of variable size and the sets the min and max points
void Drawer::calculateMinandMaxPoints(glm::vec3* array, int arrsize, glm::vec3* finmin, glm::vec3* finmax)
{

  glm::vec3 max = glm::vec3(std::numeric_limits<float>::min());
  glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());

  for(int i=0;i<arrsize;i++)
  {
    glm::vec3 curVec = array[i];
    if(curVec.x > max.x) max.x = curVec.x;
    if(curVec.y > max.y) max.y = curVec.y;
    if(curVec.z > max.z) max.z = curVec.z;
    if(curVec.x < min.x) min.x = curVec.x;
    if(curVec.y < min.y) min.y = curVec.y;
    if(curVec.z < min.z) min.z = curVec.z;
  }
  *finmin = min;
  *finmax = max;

}

void Drawer::drawTerrain(Shader* shader, const glm::mat4 &clipMat, bool useHSR)
{
  /*
  Iterates through all the chunks and draws them unless they're marked for destruciton
  then it calls freeGl which will free up all  resourses on the chunk and then
  removes all refrences to it
  At that point the chunk should be deleted by the smart pointers;
  */

  glEnable(GL_DEPTH_TEST);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, glTexture);

  useHSR = true;
  if(chunksToDraw->empty()) return;
  for(auto it = chunksToDraw->cbegin();it != chunksToDraw->cend();++it)
  {
    std::shared_ptr<BSPNode> curNode = (*it);
    if(curNode->toDelete == true)
    {
      curNode->del();
    }
    else
    {
      if(useHSR)
      {
        //Do view frustrum clipping
        int x = curNode->curBSP.xCoord*CHUNKSIZE+CHUNKSIZE/2;
        int y = curNode->curBSP.yCoord*CHUNKSIZE+CHUNKSIZE/2;
        int z = curNode->curBSP.zCoord*CHUNKSIZE+CHUNKSIZE/2;


        glm::vec4 p1 = glm::vec4(x,y,z,1);

        p1 = clipMat*p1;
        double w = p1.w;


        if(abs(p1.x) < w && abs(p1.y) < w && 0 < p1.z && p1.z < w)
          curNode->drawOpaque();
      }
      else curNode->drawOpaque();
    }

  }


}

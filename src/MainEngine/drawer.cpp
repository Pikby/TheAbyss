#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <random>
#include <limits>
#include "include/drawer.h"
#include "../TextureLoading/textureloading.h"
#include "include/world.h"

void Drawer::setupShadersAndTextures(int width, int height)
{


  const char* texture = "../assets/textures/atlas.png";
  glGenTextures(1, &glTexture);
  glBindTexture(GL_TEXTURE_2D, glTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //Load and bind the texture from the class
  int texWidth, texHeight, nrChannels;
  unsigned char* image = loadTexture(texture, &texWidth,&texHeight,&nrChannels);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,0,GL_RGBA, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  freeTexture(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  screenWidth = width;
  screenHeight = height;

  objShader = Shader("../src/Shaders/EntityShaders/entShader.vs",
                     "../src/Shaders/EntityShaders/entShader.fs");
  objShader.use();
  objShader.setInt("curTexture",0);
  objShader.setInt("dirLight.shadow",4);
  objShader.setInt("pointLights[0].shadow",5);
  objShader.setInt("pointLights[1].shadow",6);
  objShader.setFloat("far_plane",25.0f);


  depthBufferLoadingShader = Shader("../src/Shaders/depthBufferLoadingShader.vs","../src/Shaders/depthBufferLoadingShader.fs");
  depthBufferLoadingShader.use();
  depthBufferLoadingShader.setInt("depthTexture",0);

  quadShader = Shader("../src/Shaders/old/debugQuad.vs","../src/Shaders/old/debugQuad.fs");
  quadShader.use();

  //blockShader.setInt("depthMap",0);

  blockShader = Shader("../src/Shaders/BSPShaders/shaderBSP.fs","../src/Shaders/BSPShaders/shaderBSP.vs");
  blockShader.use();
  blockShader.setInt("gPosition", 0);
  blockShader.setInt("gNormal", 1);
  blockShader.setInt("gColorSpec", 2);
  blockShader.setInt("transTexture", 3);
  blockShader.setInt("dirLight.shadow[0]",4);
  blockShader.setInt("dirLight.shadow[1]",5);
  blockShader.setInt("dirLight.shadow[2]",6);
  blockShader.setInt("dirLight.shadow[3]",7);
  blockShader.setFloat("far_plane",25.0f);
  blockShader.setFloat("fog_start",CHUNKSIZE*(horzRenderDistance-2));
  blockShader.setFloat("fog_dist",CHUNKSIZE);
  gBufferShader = Shader("../src/Shaders/BSPShaders/gBuffer.fs","../src/Shaders/BSPShaders/gBuffer.vs");

  int cellWidth = 128;
  gBufferShader.use();
  gBufferShader.setInt("textureAtlasWidthInCells",texWidth/cellWidth);
  gBufferShader.setInt("textureAtlasHeightInCells",texHeight/cellWidth);
  gBufferShader.setInt("cellWidth",cellWidth);
  gBufferShader.setInt("curTexture",0);
  gBufferShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);


  transShader = Shader("../src/Shaders/BSPShaders/transShader.fs","../src/Shaders/BSPShaders/transShader.vs");


  transShader.use();
  transShader.setInt("textureAtlasWidthInCells",texWidth/cellWidth);
  transShader.setInt("textureAtlasHeightInCells",texHeight/cellWidth);
  transShader.setInt("cellWidth",cellWidth);
  transShader.setInt("curTexture",0);
  transShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);


  //std::cout << texWidth/cellWidth << ":" << texHeight/cellWidth << "\n";

  dirDepthShader   = Shader("../src/Shaders/dirDepthShader.fs",
                            "../src/Shaders/dirDepthShader.vs");
  dirDepthShader.use();
  glm::mat4 model(1.0f);
  dirDepthShader.setMat4("model",model);
  pointDepthShader = Shader("../src/Shaders/pointDepthShader.fs",
                            "../src/Shaders/pointDepthShader.vs",
                            "../src/Shaders/pointDepthShader.gs");


  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);


    // - position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // - normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // - color + specular color buffer
    glGenTextures(1, &gColorSpec);
    glBindTexture(GL_TEXTURE_2D, gColorSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);


    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    glGenTextures(1, &gDepth);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,screenWidth,screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);


  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  glGenFramebuffers(1, &transBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, transBuffer);

    glGenTextures(1, &transTexture);
    glBindTexture(GL_TEXTURE_2D, transTexture);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,screenWidth,screenHeight,0,GL_RGBA,GL_FLOAT,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, transTexture, 0);

    uint transDepth;
    glGenTextures(1, &transDepth);
    glBindTexture(GL_TEXTURE_2D, transDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,screenWidth,screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, transDepth, 0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);

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

void Drawer::renderGBuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,screenWidth,screenHeight);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    Shader* shader = &gBufferShader;
    shader->use();
    shader->setMat4("view", viewMat);

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    drawTerrainOpaque(shader,chunksToDraw);
    //drawObjects();
    //drawTerrainTranslucent(shader,chunksToDraw);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  glBindFramebuffer(GL_FRAMEBUFFER, transBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glViewport(0,0,screenWidth,screenHeight);

    depthBufferLoadingShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDepth);


    renderQuad();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    shader = &transShader;
    shader->use();
    shader->setMat4("view", viewMat);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
    drawTerrainTranslucent(shader,chunksToDraw);
    glDepthMask(GL_TRUE);
    //glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


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
void Drawer::renderQuad()
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

glm::ivec3 toChunkCoords(glm::ivec3 in)
{
  glm::ivec3 out;
  out.x = floor((float)in.x/(float)CHUNKSIZE);
  out.y = floor((float)in.y/(float)CHUNKSIZE);
  out.z = floor((float)in.z/(float)CHUNKSIZE);
  return out;
}


void Drawer::renderDirectionalShadows()
{
  const int PI = 3.14159265;
  int distToSun = (vertRenderDistance)*CHUNKSIZE;
  //Makes sure the light is always at the correct angle above the player
  glm::mat4 lightProjection,lightView, lightSpaceMatrix;

  float shadowNear = -1;
  float shadowFar = distToSun*2;

  for(int x = 0;x<NUMBOFCASCADEDSHADOWS;x++)
  {
    //if(x>=1) break;
    glm::vec3 frustrum[8];
    float near = camNear + ((camFar-camNear)/NUMBOFCASCADEDSHADOWS)*x;
    float far = camNear + ((camFar-camNear)/NUMBOFCASCADEDSHADOWS)*(x+1);
    calculateFrustrum(frustrum,viewPos,viewFront,viewRight,viewUp,camZoomInDegrees,(float)screenWidth/(float)screenHeight,near,far);
    dirLight.arrayOfDistances[x] = far;
    glm::vec3 lightTarget(0);
    glm::vec3 lightPos(0);
    for(int i=0;i<8;i++)
    {
      lightTarget+=frustrum[i];
    //  std::cout << i << glm::to_string(frustrum[i]) << "\n";
    }
    lightTarget /= 8;

    glm::vec3 sunAngle = dirLight.direction;
    lightPos = lightTarget -(sunAngle*((float)distToSun));
    objList[x]->setPosition(lightPos);
    objList[x]->setColor(glm::vec3(0.5f,0.5f,0.5f));
    objList[x]->updateModelMat();
    //std::cout << glm::to_string(lightTarget) << glm::to_string(sunAngle) << distToSun << "\n";
    //std::cout << "Lightpos"<< glm::to_string(lightPos) << "\n";

    //std::cout << "Lightpos::" << glm::to_string(lightPos) << glm::to_string(lightTarget) << "\n";
    lightView  = glm::lookAt(lightPos,lightTarget,glm::vec3(0.0f,1.0f,0.0f));
    //std::cout << "LightView: " << glm::to_string(lightView);

    glm::vec3 min,max;

    for(int i=0;i<8;i++)
    {
      //std::cout << glm::to_string(frustrum[i]) << "\n";
      //std::cout << glm::to_string(lightView) << "\n";
      frustrum[i] = glm::vec3(lightView*glm::vec4(frustrum[i],1.0f));
      //std::cout << i << glm::to_string(frustrum[i]) << "\n";
    }

    calculateMinandMaxPoints(frustrum,8,&min,&max);
    const int buf = 0;
    int factor = x == 0 ? factor = 1 : factor = x*2;
    float viewWidth = directionalShadowResolution/factor;
    lightProjection = glm::ortho(min.x, max.x, min.y,max.y,-1.0f,shadowFar);
    dirLight.lightSpaceMat[x] = lightProjection * lightView;



    dirDepthShader.use();
    dirDepthShader.setMat4("lightSpaceMatrix",dirLight.lightSpaceMat[x]);

    glm::vec3 worldMin = min;
    glm::vec3 worldMax = max;
    //std::cout << glm::to_string(min) << glm::to_string(max) << "\n";
    //std::cout << glm::to_string(worldMin) << glm::to_string(worldMax) << "\n";
    //std::cout << glm::to_string(lightPos) << "\n";
    //calculateMinandMaxPoints(frustrum,8,&min,&max);
    //auto chunkList = World::BSPmap.findAll(toChunkCoords(worldMin),toChunkCoords(worldMax));
    auto chunkList = std::make_shared<std::list<std::shared_ptr<BSPNode>>>(World::BSPmap.getFullList());
    glViewport(0,0,viewWidth,viewWidth);
      glBindFramebuffer(GL_FRAMEBUFFER, dirLight.depthMapFBO[x]);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glCullFace(GL_FRONT);
        drawTerrainOpaque(&dirDepthShader,chunkList);
        glCullFace(GL_BACK);
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

  glViewport(0,0,screenWidth,screenHeight);
  Shader* shader = &blockShader;
  shader->use();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,gPosition);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,gNormal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D,gColorSpec);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D,transTexture);

  setLights(shader);
  shader->setVec3("viewPos", viewPos);
  bindDirectionalShadows(shader);
  glDisable(GL_DEPTH_TEST);
  renderQuad();
  glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

}

void Drawer::updateViewProjection(float camZoom,float near,float far)
{

  camZoomInDegrees = camZoom;
  camNear = near == 0 ? camNear : near;
  camFar  = far  == 0 ? camFar  : far;
  viewProj = glm::perspective(glm::radians(camZoom),
                              (float)screenWidth/(float)screenHeight,
                              camNear,camFar+5*CHUNKSIZE);

  hsrProj = glm::perspective(glm::radians(camZoom+10.0f),float(screenWidth)/(float)screenHeight
                           ,camNear,camFar+CHUNKSIZE*3);
  objShader.use();
  objShader.setMat4("projection",viewProj);
  blockShader.use();
  blockShader.setMat4("projection",viewProj);
  gBufferShader.use();
  gBufferShader.setMat4("projection",viewProj);
  transShader.use();
  transShader.setMat4("projection",viewProj);
}

void Drawer::updateCameraMatrices(Camera* cam)
{
  viewMat = cam->getViewMatrix();
  hsrMat = hsrProj*cam->getHSRMatrix();
  viewPos = cam->getPosition();
  viewFront = cam->front;
  viewUp = cam->up;
  viewRight = cam->right;
  calculateFrustrum(viewFrustrum,viewPos,viewFront,viewRight,viewUp,camZoomInDegrees,(float)screenWidth/(float)screenHeight,camNear,camFar);
  calculateMinandMaxPoints(viewFrustrum,8,&viewMin,&viewMax);
}


//Caculates the viewing frustrum, likely could  be optimized, since this algorithm has more calculations then necessary, but it functions
void Drawer::calculateFrustrum(glm::vec3* arr,const glm::vec3 &pos,const glm::vec3 &front,const glm::vec3 &right,const glm::vec3 &up, float camZoomInDegrees,float ar,float near, float far)
{
  float buffer = 0;
  float fovH = glm::radians(((camZoomInDegrees+buffer)/2)*ar);
  float fovV = glm::radians((camZoomInDegrees+buffer)/2);
  glm::vec3 curPos = pos - front*((float)CHUNKSIZE*2);
  glm::vec3 rightaxis = glm::rotate(front,fovH,up);
  glm::vec3 toprightaxis = glm::rotate(rightaxis,fovV,right);
  glm::vec3 bottomrightaxis = glm::rotate(rightaxis,-fovV,right);
  glm::vec3 leftaxis = glm::rotate(front,-fovH,up);
  glm::vec3 topleftaxis = glm::rotate(leftaxis,fovV,right);
  glm::vec3 bottomleftaxis = glm::rotate(leftaxis,-fovV,right);


  //std::cout << "nnear and far" << near << ":" << far <<  "\n";
  float d;
  float a = near;
  d = a/(glm::dot(bottomleftaxis,front));
  arr[0] = d*bottomleftaxis+curPos;
  d = a/(glm::dot(topleftaxis,front));
  arr[1] = d*topleftaxis+curPos;
  d = a/(glm::dot(toprightaxis,front));
  arr[2] =  d*toprightaxis+curPos;
  d = a/(glm::dot(bottomrightaxis,front));
  arr[3] = d*bottomrightaxis+curPos;


  a = far;
  d = a/(glm::dot(bottomleftaxis,front));
  arr[4] = d*bottomleftaxis+curPos;
  d = a/(glm::dot(topleftaxis,front));
  arr[5] = d*topleftaxis+curPos;
  d = a/(glm::dot(toprightaxis,front));
  arr[6] =  d*toprightaxis+curPos;
  d = a/(glm::dot(bottomrightaxis,front));
  arr[7] = d*bottomrightaxis+curPos;


}

//Takes array of variable size and the sets the min and max points
void Drawer::calculateMinandMaxPoints(const glm::vec3* array, int arrsize, glm::vec3* finmin, glm::vec3* finmax)
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


/*
Iterates through all the chunks and draws them unless they're marked for destruciton
then it calls freeGl which will free up all  resourses on the chunk and then
removes all refrences to it
At that point the chunk should be deleted by the smart pointers;
*/

void Drawer::drawTerrainOpaque(Shader* shader,std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> list)
{
  if(list->empty()) return;
  for(auto it = list->cbegin();it != list->cend();++it)
  {
    std::shared_ptr<BSPNode> curNode = (*it);
    curNode->drawOpaque(shader,viewPos);
  }
}

//Same as draw terrain, but for translucent blocks.
void Drawer::drawTerrainTranslucent(Shader* shader,std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> list)
{
  if(list->empty()) return;
  for(auto it = list->cbegin();it != list->cend();++it)
  {
    std::shared_ptr<BSPNode> curNode = (*it);
    curNode->drawTranslucent(shader,viewPos);
  }
}

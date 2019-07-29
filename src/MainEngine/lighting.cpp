#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


#include <random>
#include <limits> 

#include "../Settings/settings.h"
#include "../Objects/include/objects.h"
#include "../TextureLoading/textureloading.h"

#include "include/drawer.h"
#include "include/bsp.h"
#include "include/world.h"

void Drawer::renderDirectionalDepthMap()
{

  for(int i=0;i<NUMBOFCASCADEDSHADOWS;i++)
  {
    std::cout << "Creating depth map\n";
    glGenFramebuffers(1, &(dirLight.depthMapFBO[i]));
    glBindFramebuffer(GL_FRAMEBUFFER, dirLight.depthMapFBO[i]);

      glGenTextures(1, &(dirLight.depthMap[i]));
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, dirLight.depthMap[i]);
      int factor = i == 0 ? factor = 1 : factor = i*2;
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT,
                   directionalShadowResolution/factor,directionalShadowResolution/factor,true);
      float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
      //glTexParameterfv(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_BORDER_COLOR, borderColor);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, dirLight.depthMap[i], 0);
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
  float start = glfwGetTime();
  shader->use();
  shader->setVec3("dirLight.direction", dirLight.direction);
  shader->setVec3("dirLight.ambient", dirLight.ambient);
  shader->setVec3("dirLight.diffuse", dirLight.diffuse);
  shader->setVec3("dirLight.specular", dirLight.specular);

  shader->setInt("numbOfCascadedShadows",NUMBOFCASCADEDSHADOWS);
  //unsigned int numbOfLights = lightList.size();
  /*
  std::list<Light> list= PlayerWorld.findAllLights(viewPos,100);

  uint numbOfLights = list.size();

  shader->setInt("numbOfLights",numbOfLights);
  //std::cout << "numboflights" << numbOfLights << "\n";
  int i =0;
  for(auto itr = list.begin();itr != list.end();itr++)
  {
    // /std::cout << i << "\n";
    Light light = *itr;
    //std::cout << glm::to_string(light.pos) << "\n";
    PointLight curLight =
    {
      light.pos,glm::vec3(1.0f,1.0f,1.0f),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0.5f,0.5f,0.5f),30.0f,1.0,0.045,0.0075
    };
    //PointLight curLight = lightList[i];
    //std::cout << glm::to_string(curLight.position) << "\n";
    shader->setVec3("pointLights[" + std::to_string(i) + "].position",curLight.position-viewPos);
    shader->setVec3("pointLights[" + std::to_string(i) + "].ambient",curLight.ambient);
    shader->setVec3("pointLights[" + std::to_string(i) + "].specular",curLight.specular);
    shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse",curLight.diffuse);
    shader->setFloat("pointLights[" + std::to_string(i) + "].radius",curLight.radius);
    shader->setFloat("pointLights[" + std::to_string(i) + "].constant",curLight.constant);
    shader->setFloat("pointLights[" + std::to_string(i) + "].linear",curLight.linear);
    shader->setFloat("pointLights[" + std::to_string(i) + "].quadratic",curLight.quadratic);
    i++;
  }
  float end = glfwGetTime();
  //std::cout << "light setup took: " << 1000*(end-start) << "ms\n";
  */
}

void Drawer::renderDirectionalShadows()
{

  const int PI = 3.14159265;
  float distToSun = (vertRenderDistance*2)*CHUNKSIZE;
  //Makes sure the light is always at the correct angle above the player
  glm::mat4 lightProjection,lightView, lightSpaceMatrix;

  float shadowNear = -1;
  float shadowFar = distToSun*2;
  glm::vec3 sunAngle = dirLight.direction;
  for(int x = 0;x<NUMBOFCASCADEDSHADOWS;x++)
  {
    float start = glfwGetTime();
    //if(x>=1) break;
    glm::vec3 frustrum[8];
    float near = camNear + ((camFar-camNear)/NUMBOFCASCADEDSHADOWS)*x;
    float far = camNear + ((camFar-camNear)/NUMBOFCASCADEDSHADOWS)*(x+1);
    calculateFrustrum(frustrum,viewPos,viewFront,viewRight,viewUp,camZoomInDegrees,(float)screenWidth/(float)screenHeight,near,far);


    glm::vec3 vec1 = frustrum[3]-sunAngle*distToSun;;
    glm::vec3 vec2 = frustrum[4];

    glm::vec3 worldMin;
    glm::vec3 worldMax;

    calculateMinandMaxPoints(frustrum,8,&worldMin,&worldMax);
    worldMax -= sunAngle*distToSun;


    dirLight.arrayOfDistances[x] = far;
    glm::vec3 lightTarget(0);
    glm::vec3 lightPos(0);
    for(int i=0;i<8;i++)
    {
      lightTarget+=frustrum[i];

    }
    lightTarget /= 8;


    lightPos = lightTarget  -sunAngle*distToSun;

    glm::vec3 finalMin;
    glm::vec3 finalMax;

    finalMin.x = min(lightTarget.x,worldMin.x);
    finalMax.x = max(lightTarget.x,worldMax.x);
    finalMin.y = min(lightTarget.y,worldMin.y);
    finalMax.y = max(lightTarget.y,worldMax.y);
    finalMin.z = min(lightTarget.z,worldMin.z);
    finalMax.z = max(lightTarget.z,worldMax.z);

    lightView  = glm::lookAt(lightPos-viewPos,lightTarget-viewPos,glm::vec3(0.0f,1.0f,0.0f));


    glm::vec3 min,max;

    for(int i=0;i<8;i++)
    {
      frustrum[i] = glm::vec3(lightView*glm::vec4(frustrum[i],1.0f));
    }

    calculateMinandMaxPoints(frustrum,8,&min,&max);
    const int buf = 0;
    int factor = x == 0 ? factor = 1 : factor = x*2;
    float viewWidth = directionalShadowResolution/factor;


    glm::vec3 lightPosLightSpace = glm::vec3(lightView*glm::vec4(lightTarget,1.0f));
    glm::vec3 test = glm::vec3(lightView*glm::vec4((lightTarget-viewPos)*sunAngle,1.0f));
    // /std::cout << glm::to_string(lightPosLightSpace) << glm::to_string(test) << "\n";
    float buffer = 32;
    float l = -buffer -abs(lightPosLightSpace.x - min.x);
    float r = buffer + abs(lightPosLightSpace.x - max.x);
    float b = -buffer - abs(lightPosLightSpace.y - min.y);
    float t = buffer + abs(lightPosLightSpace.y - max.y);



    //std::cout << l << ";" << r << ";" <<  b << ";" << t << "\n";
    //std::cout << glm::to_string(min) << glm::to_string(max) << "\n";
    //std::cout << glm::to_string(lightTarget) << "\n";
    //s/td::cout << glm::to_string(min) << glm::to_string(max) << "\n";

    glm::vec3 bottomLightTarget = lightTarget;
    lightProjection = glm::ortho(l,r,b,t,-1.0f,shadowFar);
    dirLight.lightSpaceMat[x] = lightProjection * lightView;
    glm::mat4 invLight = glm::inverse(dirLight.lightSpaceMat[x]);

    const glm::vec3 squareFrustrum[8]=
    {
      glm::vec3(-1.0f,-1.0f,-1.0f),
      glm::vec3(-1.0f,1.0f,-1.0f),
      glm::vec3(1.0f,1.0f,-1.0f),
      glm::vec3(1.0f,-1.0f,-1.0f),

      glm::vec3(-1.0f,-1.0f,1.0f),
      glm::vec3(-1.0f,1.0f,1.0f),
      glm::vec3(1.0f,1.0f,1.0f),
      glm::vec3(1.0f,-1.0f,1.0f),

    };

    glm::vec3 lightFrustrum[8];
    for(int i=0;i<8;i++)
    {
      lightFrustrum[i] = viewPos + glm::vec3(invLight*glm::vec4(squareFrustrum[i],1.0f));
      //std::cout <<i << " : " << glm::to_string(lightFrustrum[i]) << "\n";
    }


    calculateMinandMaxPoints(lightFrustrum,8,&finalMin,&finalMax);
    dirDepthShader->use();
    dirDepthShader->setMat4("lightSpaceMatrix",dirLight.lightSpaceMat[x]);


    auto check = [](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 target)
    {
        glm::vec3 U = b - a;
        glm::vec3 V = c - a;
        glm::vec3 UVCross = glm::cross(U,V);
        glm::vec3 chunkVec = target - a;
        return (glm::dot(chunkVec,UVCross) < 0);
    };
    //auto chunkList = std::make_shared<std::list<std::shared_ptr<BSPNode>>>(BSPmap.getFullList());
    auto chunkList = PlayerWorld.BSPmap.findAll(toChunkCoords(finalMin),toChunkCoords(finalMax));

    /*
    if(chunkList->empty()) return;
    for(auto it = chunkList->cbegin();it != chunkList->cend();)
    {
      std::shared_ptr<BSPNode> curNode = (*it);
      glm::vec3 chunkVec = curNode->getRealWorldPosition();
      if(
         !check(lightFrustrum[3],lightFrustrum[6],lightFrustrum[7],chunkVec)
         && !check(lightFrustrum[3],lightFrustrum[0],lightFrustrum[2],chunkVec)
         && !check(lightFrustrum[0],lightFrustrum[4],lightFrustrum[5],chunkVec)
         && !check(lightFrustrum[2],lightFrustrum[5],lightFrustrum[6],chunkVec)
         && !check(lightFrustrum[3],lightFrustrum[7],lightFrustrum[4],chunkVec)
       )
      {
        //std::cout << glm::to_string(chunkVec) << "Is in\n";
        ++it;
      }
      else
      {
        //std::cout << glm::to_string(chunkVec) << "Is out\n";
        it = (chunkList->erase(it));
      }

    }
    //std::cout << chunkList->size() << "\n";

    */

    glViewport(0,0,viewWidth,viewWidth);
      glBindFramebuffer(GL_FRAMEBUFFER, dirLight.depthMapFBO[x]);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glCullFace(GL_FRONT);
        //drawTerrainOpaque(dirDepthShader.get(),chunkList);
        glCullFace(GL_BACK);
      glBindFramebuffer(GL_FRAMEBUFFER,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float end = glfwGetTime();

    //std::cout << "Calculating took" << 1000*(end-start) << "\n";
  }
  glViewport(0,0,screenWidth,screenHeight);

}


void Drawer::bindDirectionalShadows(Shader* shader)
{
  shader->use();
  for(int i=0;i<NUMBOFCASCADEDSHADOWS;i++)
  {
    glActiveTexture(GL_TEXTURE4+i);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, dirLight.depthMap[i]);
    shader->setMat4("dirLight.lightSpaceMatrix[" +std::to_string(i) + "]",dirLight.lightSpaceMat[i]);
    shader->setFloat("dirLight.arrayOfDistances[" + std::to_string(i) + "]",dirLight.arrayOfDistances[i]);
  }
}

void Drawer::renderPointShadows()
{
  pointDepthShader->use();
  for(int i=0;i<objList.size();i++)
  {
    PointLight* light = &(lightList[i]);
    glViewport(0, 0, directionalShadowResolution, directionalShadowResolution);
    glBindFramebuffer(GL_FRAMEBUFFER,light->depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      pointDepthShader->setVec3("lightPos",light->position);
      pointDepthShader->setFloat("far_plane", 25.0f);

      for(int i=0;i<6;i++)
      {
        pointDepthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", light->shadowTransform[i]);
      }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
  }
}

void Drawer::bindPointShadows()
{
  objShader->use();
  unsigned int numbOfLights = lightList.size();
  for(uint i=0;i<numbOfLights;i++)
  {
    PointLight* curLight= &(lightList[i]);
    glActiveTexture(GL_TEXTURE5+i);
    glBindTexture(GL_TEXTURE_CUBE_MAP,curLight->depthCubemap);
  }
}

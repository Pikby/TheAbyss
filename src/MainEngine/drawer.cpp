#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


#include <random>
#include <limits>
#define GLSLSHADERSIMPLEMNTATION

#include "../Settings/settings.h"
#include "../Objects/include/objects.h"
#include "../TextureLoading/textureloading.h"
#include "../Character/include/mainchar.h"

#include "include/drawer.h"
#include "include/bsp.h"
#include "include/world.h"



Drawer::Drawer()
{
  int vertRenderDistance = std::stoi(Settings::get("horzRenderDistance"));
  int horzRenderDistance = std::stoi(Settings::get("vertRenderDistance"));
  int renderBuffer = std::stoi(Settings::get("renderBuffer"));
  setRenderDistances(vertRenderDistance,horzRenderDistance,renderBuffer);

  int width = std::stoi(Settings::get("windowWidth"));
  int height = std::stoi(Settings::get("windowHeight"));
  setupShadersAndTextures(width,height);

}

void Drawer::createTextureAtlas(const char* texture,int cellWidth)
{
  glGenTextures(1, &textureAtlas);
  glBindTexture(GL_TEXTURE_2D, textureAtlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //Load and bind the texture from the class
    int texWidth,texHeight,nrChannels;
    unsigned char* image = loadTexture(texture, &texWidth,&texHeight,&nrChannels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,0,GL_RGBA, GL_UNSIGNED_BYTE, image);

    std::cout << texWidth << ":" << texHeight << "\n";
    //Max mipmaplevel is required in order to stop texture bleeding on very small mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 7);
    glGenerateMipmap(GL_TEXTURE_2D);
    textureAtlasDimensions = glm::ivec2(texWidth,texHeight);
    freeTexture(image);

  glBindTexture(GL_TEXTURE_2D, 0);
}


void Drawer::setupShadersAndTextures(int width, int height)
{

  try
  {


  createTextureAtlas("../assets/textures/atlas.png",128);
  screenWidth = width;
  screenHeight = height;

  skyBox = SkyBox("../assets/textures/skybox/sky");
  Object::initializeObjectShader();

  depthBufferLoadingShader = std::make_unique<Shader>(Shader("depthBufferLoadingShader.vs","depthBufferLoadingShader.fs"));
  depthBufferLoadingShader->use();

  std::cout << "Blockshader\n";
  blockShader = std::make_unique<Shader>(Shader("BSPShaders/shaderBSP.fs","BSPShaders/shaderBSP.vs"));
  blockShader->use();
  blockShader->setInt("gPosition", 0);
  blockShader->setInt("gNormal", 1);
  blockShader->setInt("gColorSpec", 2);
  blockShader->setInt("transTexture", 3);
  blockShader->setInt("dirLight.shadow[0]",4);
  blockShader->setInt("dirLight.shadow[1]",5);
  blockShader->setInt("dirLight.shadow[2]",6);
  blockShader->setInt("dirLight.shadow[3]",7);
  blockShader->setFloat("far_plane",25.0f);
  blockShader->setFloat("fog_start",CHUNKSIZE*(std::stoi(Settings::get("horzRenderDistance"))-2));
  blockShader->setFloat("fog_dist",CHUNKSIZE);
  blockShader->setIVec2("resolution",glm::ivec2(screenWidth,screenHeight));
  blockShader->setBool("shadowsOn",true);

  BSP::initializeBSPShader(textureAtlasDimensions);
  setAllBuffers();


  }
  catch(...)
  {
    std::cout << "Error setting up shaders\n";
    exit(-1);
  }

}

void Drawer::deleteAllBuffers()
{
  glDeleteBuffers(1,&gBuffer);
  glDeleteTextures(1,&gDepth);
  glDeleteTextures(1,&gPosition);
  glDeleteTextures(1,&gNormal);
  glDeleteTextures(1,&gColorSpec);

  glDeleteBuffers(1,&transBuffer);
  glDeleteTextures(1,&transTexture);
  glDeleteTextures(1,&transDepth);

}


void Drawer::genMultisampleTexture(uint* name,int samples,GLenum attachment)
{
  glGenTextures(1,name);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,*name);

  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples ,GL_RGB16F, screenWidth, screenHeight,true);
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, *name, 0);
}

void Drawer::setAllBuffers()
{

  int samples = 1;
  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    genMultisampleTexture(&gPosition,1,GL_COLOR_ATTACHMENT0);
    genMultisampleTexture(&gNormal,1,GL_COLOR_ATTACHMENT1);
    genMultisampleTexture(&gColorSpec,1,GL_COLOR_ATTACHMENT2);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);


    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);



  glGenFramebuffers(1, &transBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, transBuffer);
     genMultisampleTexture(&transTexture,8,GL_COLOR_ATTACHMENT0);
     glGenTextures(1, &transDepth);
     glBindTexture(GL_TEXTURE_2D, transDepth);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,screenWidth,screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, transDepth, 0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
       std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
 glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Drawer::setRenderDistances(int vert, int horz, int buffer)
{
  /*
  vertRenderDistance = vert;
  horzRenderDistance = horz;
  renderBuffer = buffer;
*/
}
void Drawer::addCube(Cube newCube)
{
  objList.push_back(std::make_shared<Cube>(newCube));
}

void Drawer::setExposure(float exposure)
{
  /*
  PPShader->use();
  PPShader->setFloat("exposure",exposure);
  */
}


void Drawer::renderGBuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,screenWidth,screenHeight);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureAtlas);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glm::mat4 view = MainChar::getCamera().getViewMatrix();
    skyBox.draw(view);
    drawTerrainOpaque(chunksToDraw);
    MainChar::drawPreviewBlock();
    drawObjects();
    drawPlayers();

    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  /*
  glBindFramebuffer(GL_FRAMEBUFFER, transBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(-1.0f,-1.0f,-1.0f,1.0f);
    glViewport(0,0,screenWidth*MSAA,screenHeight*MSAA);

    depthBufferLoadingShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    renderQuad();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureAtlas);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    shader = transShader.get();
    shader->use();
    shader->setMat4("view", viewMat);
    glDepthMask(GL_FALSE);
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
    drawTerrainTranslucent(shader,chunksToDraw);
    glDepthMask(GL_TRUE);
    //glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  */

  glFlush();
}



void Drawer::renderQuad()
{
  static unsigned int quadVAO = 0;
  static unsigned int quadVBO;
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

glm::ivec3 Drawer::toChunkCoords(const glm::ivec3& in)
{
  glm::ivec3 out;
  out.x = floor((float)in.x/(float)CHUNKSIZE);
  out.y = floor((float)in.y/(float)CHUNKSIZE);
  out.z = floor((float)in.z/(float)CHUNKSIZE);
  return out;
}




void Drawer::drawObjects()
{

  glViewport(0,0,screenWidth,screenHeight);


  glBindTexture(GL_TEXTURE_CUBE_MAP,0);
  glBindTexture(GL_TEXTURE_2D,0);
  glm::vec3 viewPos = MainChar::getCamera().getPosition();
  Object::updateMatrices(MainChar::getCamera());


  for(int i=0;i<objList.size();i++)
  {
    //std::cout << objList.size() << "\n";
    objList[i]->draw(viewPos);
  }
}


void Drawer::drawPlayers()
{
  glViewport(0,0,screenWidth,screenHeight);
  glBindTexture(GL_TEXTURE_CUBE_MAP,0);
  glBindTexture(GL_TEXTURE_2D,0);
  glm::vec3 viewPos = MainChar::getCamera().getPosition();
  for(auto it = playerList.begin(); it != playerList.end();it++)
  {
    it->second->draw(viewPos);
  }
}

void Drawer::drawFinal()
{

  glViewport(0,0,screenWidth,screenHeight);
  Shader* shader = blockShader.get();
  shader->use();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 //glBindFramebuffer(GL_FRAMEBUFFER, PPBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,gColorSpec);
    ///glActiveTexture(GL_TEXTURE3);
    //glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,gPosition);

    setLights(shader);
    shader->setVec3("viewPos", MainChar::getCamera().getPosition());
    //bindDirectionalShadows(shader);


    glDisable(GL_DEPTH_TEST);
      renderQuad();
    glEnable(GL_DEPTH_TEST);


}




void Drawer::updateShadows(bool val)
{
  blockShader->use();
  blockShader->setBool("shadowsOn",val);
}




//Takes array of variable size and the sets the min and max points
void Drawer::calculateMinandMaxPoints(const glm::vec3* array, int arrsize, glm::vec3* finmin, glm::vec3* finmax)
{

  glm::vec3 max = -glm::vec3(std::numeric_limits<float>::max());
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


void Drawer::addLight(glm::vec3 pos,glm::vec3 amb,glm::vec3 spe,glm::vec3 dif,
         float cons, float lin, float quad)
{
  lightList.push_back(PointLight{pos,amb,spe,dif,cons,lin,quad});
  //renderPointDepthMap(lightList.size()-1);
}


void Drawer::updateDirectionalLight(glm::vec3 dir,glm::vec3 amb,glm::vec3 dif,glm::vec3 spec)
{
  dirLight.direction = glm::normalize(dir);
  dirLight.ambient = amb;
  dirLight.diffuse = dif;
  dirLight.specular =spec;
}


void Drawer::drawTerrainOpaque(std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> list)
{
  if(list->empty()) return;

  glm::vec3 viewPos = MainChar::getCamera().getPosition();
  BSP::updateMatrices(MainChar::getCamera());
  for(auto it = list->cbegin();it != list->cend();++it)
  {
    std::shared_ptr<BSPNode> curNode = (*it);
    curNode->drawOpaque(viewPos);
    if(drawChunkOutlines) curNode->drawChunkOutline(viewPos);
  }


}

//Same as draw terrain, but for translucent blocks.
void Drawer::drawTerrainTranslucent(std::shared_ptr<std::list<std::shared_ptr<BSPNode>>> list)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureAtlas);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  if(list->empty()) return;
  for(auto it = list->cbegin();it != list->cend();++it)
  {

    std::shared_ptr<BSPNode> curNode = (*it);
    curNode->drawTranslucent(MainChar::getCamera().getPosition());
  }
}

void Drawer::setTerrainColor(const glm::vec3 &color)
{
  BSP::setTerrainColor(color);
}

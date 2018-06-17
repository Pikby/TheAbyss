#ifdef _WIN32
  /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
  #include <unistd.h> /* Needed for close() */
#endif
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include "../headers/world.h"
#include "../headers/bsp.h"
#include "../headers/objects.h"
#include "../headers/parser.h"


#include "../TextureLoading/textureloading.h"

#include "worldMsg.h"

//#define ADDRESS "154.5.254.242"


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


void World::setLights(Shader* shader)
{

  shader->setVec3("dirLight.direction", dirLight.direction);
  shader->setVec3("dirLight.ambient", dirLight.ambient);
  shader->setVec3("dirLight.diffuse", dirLight.diffuse);
  shader->setVec3("dirLight.specular", dirLight.specular);


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
      drawTerrain(&dirDepthShader);
    }
  glBindFramebuffer(GL_FRAMEBUFFER,0);

}
void World::bindDirectionalShadows(Shader* shader)
{
  glActiveTexture(GL_TEXTURE4);

  shader->use();
  glBindTexture(GL_TEXTURE_2D, dirLight.depthMap);
  shader->setMat4("lightSpace",dirLight.lightSpaceMat);
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



World::World(int numbBuildThreads,int width,int height)
{

  typedef std::queue<std::shared_ptr<BSPNode>> buildType;
  buildQueue = new buildType[numbBuildThreads];
  numbOfThreads = numbBuildThreads;
  glm::mat4 projectMat = glm::perspective(glm::radians(45.0f),
                  (float)1920/ (float)1080, 0.1f, 100.0f);
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
  blockShader.setInt("dirLight.shadow",4);
  blockShader.setInt("pointLights[0].shadow",5);
  blockShader.setInt("pointLights[1].shadow",6);
  blockShader.setMat4("projection",projectMat);
  blockShader.setFloat("far_plane",25.0f);



  dirDepthShader   = Shader("../src/Shaders/dirDepthShader.fs",
                            "../src/Shaders/dirDepthShader.vs");

  pointDepthShader = Shader("../src/Shaders/pointDepthShader.fs",
                            "../src/Shaders/pointDepthShader.vs",
                            "../src/Shaders/pointDepthShader.gs");

  const char* texture = "../assets/textures/atlas.png";
  ItemDatabase::loadBlockDictionary("../assets/blockDictionary.dat");

  Parser settings("../assets/settings.ini");
  settings.print();
  try
  {
    horzRenderDistance = std::stoi(settings.get("horzRenderDistance"));
    vertRenderDistance = std::stoi(settings.get("vertRenderDistance"));
    renderBuffer = std::stoi(settings.get("renderBuffer"));
  }
  catch(...)
  {
    std::cout << "bad settings lul\n";
  }
  worldName = settings.get("worldName");
  std::string ipAddress = settings.get("ipAddress");

  boost::filesystem::create_directory("saves");
  boost::filesystem::create_directory("saves/"+worldName);
  boost::filesystem::create_directory("saves/"+worldName+"/chunks");

  drawnChunks = 0;
  frontNode = NULL;

  totalChunks = 0;

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

  setupSockets(ipAddress);

}


void World::movePlayer(float x, float y, float z, uchar id)
{
  //std::cout << "Moving player" << x << y << z << '\n';
  std::shared_ptr<Object> tmp = playerList[id];
  if(tmp != NULL)
  {
    //std::cout << "Moving player" << x << y << z << '\n';
    playerList[id]->setPosition(glm::vec3(x,y,z));
  }
}


void World::addPlayer(float x, float y, float z, uchar id)
{
  std::cout << "Adding player at" << x << ":" << y << ":" << z << ":"<< (int) id << "\n";
  std::shared_ptr<Player> temp(new Player(glm::vec3(x,y,z)));
  playerList[id] = temp;
}


void World::removePlayer(uchar id)
{
  std::cout << "Removing PLayer\n";
  playerList.erase(id);
}



void World::drawPlayers(Shader* shader)
{
  //std::cout << "Map has: " << playerList.size() << "\n";
  for(auto it = playerList.begin(); it != playerList.end();it++)
  {
    it->second->draw(shader);
  }
}

void World::drawTerrain(Shader* shader)
{
  /*
  Iterates through all the chunks and draws them unless they're marked for destruciton
  then it calls freeGl which will free up all  resourses on the chunk and then
  removes all refrences to it
  At that point the chunk should be deleted by the smart pointers;
  */
  if(shader == NULL) shader = &blockShader;

  glm::mat4 hsrProj = glm::perspective(glm::radians(50.0f),
                            (float)1920/ (float)1080, 0.1f,
                            (float)horzRenderDistance*CHUNKSIZE*4);
  bool useHSR = true;
  shader->use();
  setLights(shader);
  viewProj = glm::perspective(glm::radians(45.0f),
                            (float)1920/ (float)1080, 1.0f,
                            (float)horzRenderDistance*CHUNKSIZE*4);
  shader->setMat4("projection",viewProj);
  shader->setMat4("view", viewMat);
  shader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
  shader->setVec3("lightColor",  1.0f, 1.0f, 1.0f);
  shader->setVec3("lightPos",  glm::vec3(100,100,100));
  shader->setVec3("viewPos", viewPos);
  //blockShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
  glEnable(GL_DEPTH_TEST);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, glTexture);



  std::shared_ptr<BSPNode>  curNode = frontNode;
  std::shared_ptr<BSPNode>  nextNode;

  glm::mat4 mat = hsrProj*hsrMat;
  while(curNode != NULL)
  {
    nextNode = curNode->nextNode;
    if(curNode->toDelete == true)
    {
      if(curNode->prevNode == NULL) frontNode = curNode->nextNode;
      curNode->del();
    }
    else
    {
      if(useHSR)
      {
        int x = curNode->curBSP.xCoord*CHUNKSIZE+CHUNKSIZE/2;
        int y = curNode->curBSP.yCoord*CHUNKSIZE+CHUNKSIZE/2;
        int z = curNode->curBSP.zCoord*CHUNKSIZE+CHUNKSIZE/2;


        glm::vec4 p1 = glm::vec4(x,y,z,1);

        p1 = mat*p1;
        double w = p1.w;


        if(abs(p1.x) < w && abs(p1.y) < w && 0 < p1.z && p1.z < w)
          curNode->drawOpaque();
      }
      else curNode->drawOpaque();
    }
    curNode = nextNode;
  }
}

void World::generateChunkFromString(int chunkx, int chunky, int chunkz,std::string value)
{
  //std::cout << "Generating chunk" << chunkx << ":" << chunky << ":" << chunkz << "\n";
  if(chunkExists(chunkx,chunky,chunkz))
  {
    std::cout << "Replacing CHunk \n";
    delChunk(chunkx,chunky,chunkz);
  }
  std::shared_ptr<BSPNode>  tempChunk(new BSPNode(chunkx,chunky,chunkz,worldName,value));
  BSPmap.add(chunkx,chunky,chunkz,tempChunk);

  if(frontNode == NULL)
  {
    frontNode = tempChunk;
  }
  else
  {
    tempChunk->nextNode = frontNode;
    frontNode->prevNode = tempChunk;
    frontNode = tempChunk;
  }


  /*
    At this point the chunk is in both the linked list as well as the unorderedmap
    now it will attempt to find any nearby chunks and store a refrence to it
  */
  std::shared_ptr<BSPNode>  otherChunk = getChunk(chunkx,chunky,chunkz-1);
  if(otherChunk  != NULL )
  {
    tempChunk->frontChunk = otherChunk;
    otherChunk->backChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky,chunkz+1);
  if(otherChunk != NULL)
  {
    tempChunk->backChunk = otherChunk;
    otherChunk->frontChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky-1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->bottomChunk = otherChunk;
    otherChunk->topChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky+1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->topChunk = otherChunk;
    otherChunk->bottomChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx-1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->leftChunk = otherChunk;
    otherChunk->rightChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx+1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->rightChunk = otherChunk;
    otherChunk->leftChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }
  //Adds the chunk to the current mainBuildQueue if it is not already;
  if(!tempChunk->toBuild)
  {
      addToBuildQueue(tempChunk);
  }
}


void World::addToBuildQueue(std::shared_ptr<BSPNode> curNode)
{
  static int currentThread = 0;
  buildQueue[currentThread].push(curNode);
  curNode->toBuild = true;

  currentThread++;
  if(currentThread >= numbOfThreads) currentThread = 0;
}


void World::generateChunk(int chunkx, int chunky, int chunkz)
{
  if(chunkExists(chunkx,chunky,chunkz)) return;
  requestChunk(chunkx,chunky,chunkz);
  std::shared_ptr<BSPNode>  tempChunk(new BSPNode(chunkx,chunky,chunkz,worldName));
  BSPmap.add(chunkx,chunky,chunkz,tempChunk);
  if(frontNode == NULL)
  {
    frontNode = tempChunk;
  }
  else
  {
    tempChunk->nextNode = frontNode;
    frontNode->prevNode = tempChunk;
    frontNode = tempChunk;
  }


  /*
    At this point the chunk is in both the linked list as well as the unorderedmap
    now it will attempt to find any nearby chunks and store a refrence to it
  */
  std::shared_ptr<BSPNode>  otherChunk = getChunk(chunkx,chunky,chunkz-1);
  if(otherChunk  != NULL )
  {
    tempChunk->frontChunk = otherChunk;
    otherChunk->backChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky,chunkz+1);
  if(otherChunk != NULL)
  {
    tempChunk->backChunk = otherChunk;
    otherChunk->frontChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky-1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->bottomChunk = otherChunk;
    otherChunk->topChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx,chunky+1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->topChunk = otherChunk;
    otherChunk->bottomChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }


  otherChunk = getChunk(chunkx-1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->leftChunk = otherChunk;
    otherChunk->rightChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(chunkx+1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->rightChunk = otherChunk;
    otherChunk->leftChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);;
    }
  }
  //Adds the chunk to the current mainBuildQueue if it is not already;
  if(!tempChunk->toBuild)
  {
      addToBuildQueue(tempChunk);
  }
}

void World::renderWorld(float* mainx, float* mainy, float* mainz)
{
  for(int curDis = 0; curDis < horzRenderDistance; curDis++)
   {
     for(int height = 0; height < vertRenderDistance; height++)
     {

       int x = round(*mainx);
       int y = round(*mainy);
       int z = round(*mainz);
       x/= CHUNKSIZE;
       y/= CHUNKSIZE;
       z/= CHUNKSIZE;
       y--;
       int ychunk;
       if(height % 2 == 1) ychunk = y -(height/2);
       else ychunk = y + (height/2);

       int xchunk = x;
       int zchunk = z;


       zchunk = z+curDis;
       xchunk = x;
       for(int i = -curDis+1;i<curDis;i++)
       {
         createChunkRequest(xchunk+i,ychunk,zchunk);
       }

       zchunk = z-curDis;
       xchunk = x;
       for(int i = -curDis+1;i<curDis;i++)
       {
         createChunkRequest(xchunk+i,ychunk,zchunk);
       }
       zchunk = z;
       xchunk = x+curDis;
       for(int i = -curDis;i<=curDis;i++)
       {
         createChunkRequest(xchunk,ychunk,zchunk+i);
       }
       zchunk = z;
       xchunk = x-curDis;
       for(int i = -curDis;i<=curDis;i++)
       {
         createChunkRequest(xchunk,ychunk,zchunk+i);
       }
     }
   }
}

void World::buildWorld(int threadNumb)
{
  //Empties the build queue
  //TODO Break up queue into smaller pieces and use seperate threads to build them
  while(!buildQueue[threadNumb].empty())
  {
    std::shared_ptr<BSPNode> chunk = buildQueue[threadNumb].front();
    chunk->build();
    buildQueue[threadNumb].pop();
  }
}

std::shared_ptr<BSPNode>  World::getChunk(int x, int y, int z)
{
  return BSPmap.get(x,y,z);
}



void World::delChunk(int x, int y, int z)
{
  std::shared_ptr<BSPNode>  tempChunk = getChunk(x,y,z);
  if(tempChunk != NULL)
  {
    BSPmap.del(x,y,z);
    requestMap.del(x,y,z);
    tempChunk->disconnect();
    /*
    Essentially marks the chunk for death
    Removes all connected pointers besides the linked list
    The Node however must be destroyed in the draw string
    since that is the sole string using opengl functions;
    othereise the vao wont get freed causing a memory leak
    */

  }
}


void World::delScan(float* mainx, float* mainy, float* mainz)
{
  /*
    Scans through the same list as the draw function
    however this time referencing the current position of the player
    and raises the destroy flag if its out of a certain range
  */
  std::shared_ptr<BSPNode>  curNode = frontNode;
  while(curNode != NULL)
  {
    int x = round(*mainx);
    int y = round(*mainy);
    int z = round(*mainz);

    x/= CHUNKSIZE;
    y/= CHUNKSIZE;
    z/= CHUNKSIZE;

    int chunkx = curNode->curBSP.xCoord;
    int chunky = curNode->curBSP.yCoord;
    int chunkz = curNode->curBSP.zCoord;

    curNode = curNode->nextNode;
    if(abs(chunkx-x) > horzRenderDistance + renderBuffer
    || abs(chunky-y) > horzRenderDistance + renderBuffer
    || abs(chunkz-z) > horzRenderDistance + renderBuffer)
    {
      delChunk(chunkx,chunky,chunkz);
    }
  }
}

void World::saveWorld()
{
    std::shared_ptr<BSPNode>  curNode = frontNode;
    while(curNode != NULL)
    {
      curNode->saveChunk();
      curNode = curNode->nextNode;
    }
}

bool World::chunkExists(int x ,int y, int z)
{
  return BSPmap.exists(x,y,z);
}

//If there is an entity, returns it id and position,
//If there is a block, return its position and an id of 0
//If there is nothing return a 0 vector  with -1 id
glm::vec4 World::rayCast(glm::vec3 pos, glm::vec3 front, int max)
{
  float parts = 10;
  for(float i = 0; i<max;i += 1/parts)
  {
    glm::vec3 curPos = pos+i*front;
    //std::cout << curPos.x << ":" << curPos.y << ":" << curPos.z << "\n";
/*
    int id = entityExists(curPos);
    if(id != 0)
    {
      return glm::vec4(curPos,BLOCK);
    }
    */
    if(blockExists(curPos))
    {
        return glm::vec4(curPos,0);
    }
  }
  return glm::vec4(0,0,0,-1);
}



glm::ivec3 inline toLocalCoords(glm::ivec3 in)
{
  glm::ivec3 out;
  out.x = in.x >= 0 ? in.x % CHUNKSIZE : CHUNKSIZE + (in.x % CHUNKSIZE);
  out.y = in.y >= 0 ? in.y % CHUNKSIZE : CHUNKSIZE + (in.y % CHUNKSIZE);
  out.z = in.z >= 0 ? in.z % CHUNKSIZE : CHUNKSIZE + (in.z % CHUNKSIZE);
  if(out.x == CHUNKSIZE) out.x = 0;
  if(out.y == CHUNKSIZE) out.y = 0;
  if(out.z == CHUNKSIZE) out.z = 0;

  return out;
}

glm::ivec3 inline toChunkCoords(glm::ivec3 in)
{
  glm::ivec3 out;
  out.x = floor((float)in.x/(float)CHUNKSIZE);
  out.y = floor((float)in.y/(float)CHUNKSIZE);
  out.z = floor((float)in.z/(float)CHUNKSIZE);
  return out;
}

inline void checkForUpdates(glm::ivec3 local,std::shared_ptr<BSPNode> chunk)
{
  if(local.x+1>=CHUNKSIZE)
  {
    if(chunk->rightChunk != NULL) chunk->rightChunk->build();
  }
  else if(local.x-1 < 0)
  {
    if(chunk->leftChunk != NULL) chunk->leftChunk->build();
  }

  if(local.y+1>=CHUNKSIZE)
  {
    if(chunk->topChunk != NULL) chunk->topChunk->build();
  }
  else if(local.y-1 < 0)
  {
    if(chunk->bottomChunk != NULL) chunk->bottomChunk->build();
  }

  if(local.z+1>=CHUNKSIZE)
  {
    if(chunk->backChunk != NULL) chunk->backChunk->build();
  }
  else if(local.z-1 < 0)
  {
    if(chunk->frontChunk != NULL) chunk->frontChunk->build();
  }
}

bool World::blockExists(int x, int y, int z)
{
  /*
  Finds which ever chunk holds the block and then calls the blockExists
  function on that chunk with the localized coordinates
  */
  glm::ivec3 pos(x,y,z);
  glm::ivec3 local = toLocalCoords(pos);
  glm::ivec3 chunk = toChunkCoords(pos);

  std::shared_ptr<BSPNode>  tempChunk;
  if(tempChunk = getChunk(chunk.x,chunk.y,chunk.z))
  {

    if(tempChunk->blockExists(local.x,local.z,local.z))
    {
      return true;
    }
  }
  return false;
}

void World::addBlock(int x, int y, int z, int id)
{
  glm::ivec3 pos(x,y,z);
  glm::ivec3 local = toLocalCoords(pos);
  glm::ivec3 chunk = toChunkCoords(pos);

  std::shared_ptr<BSPNode> tempChunk = getChunk(chunk.x,chunk.y,chunk.z);
  if(tempChunk != NULL)
  {
    tempChunk->addBlock(local.x,local.y,local.z,id);
    tempChunk->build();
    checkForUpdates(local,tempChunk);
  }

}


void World::delBlock(int x, int y, int z)
{
  glm::ivec3 pos(x,y,z);
  glm::ivec3 local = toLocalCoords(pos);
  glm::ivec3 chunk = toChunkCoords(pos);

  std::shared_ptr<BSPNode>  tempChunk = getChunk(chunk.x,chunk.y,chunk.z);
  if(tempChunk != NULL)
  {
    tempChunk->delBlock(local.x,local.y,local.z);
    tempChunk->build();
    checkForUpdates(local,tempChunk);
  }
}


void World::updateBlock(int x, int y, int z)
{
  glm::ivec3 pos(x,y,z);
  glm::ivec3 chunk = toChunkCoords(pos);
  std::shared_ptr<BSPNode>  tempChunk = getChunk(chunk.x,chunk.y,chunk.z);
  if(!tempChunk->toBuild)
  {
    addToBuildQueue(tempChunk);
  }

}


int World::anyExists(glm::vec3 pos)
{
  if(entityExists(pos.x,pos.y,pos.z))
    return 2;
  else if( blockExists(pos.x,pos.y,pos.z))
    return 1;
  else return 0;

}

bool World::entityExists(glm::vec3 pos)
{
  //TODO
  return false;
}
bool World::entityExists(float x, float y, float z)
{
  //TODO
  return false;
}

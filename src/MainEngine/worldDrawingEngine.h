void World::addCube(Cube newCube)
{
  objList.push_back(std::make_shared<Cube>(newCube));
}

void World::renderDirectionalDepthMap()
{

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

void World::renderDirectionalShadows()
{
  const int PI = 3.14159265;
  double sunAngle = 80;
  int distToSun = (vertRenderDistance/2+1)*CHUNKSIZE;
  //Makes sure the light is always at the correct angle above the player
  glm::vec3 lightPos;
  lightPos.x = cos(sunAngle*PI/180)*distToSun+viewPos.x;
  lightPos.y = sin(sunAngle*PI/180)*distToSun+viewPos.y;
  lightPos.z = viewPos.z;
  float camNear = -1;
  float camFar = distToSun*2;
  glm::mat4 lightProjection,lightView, lightSpaceMatrix;
  float orthoSize = (3)*CHUNKSIZE;
  lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize,orthoSize,camNear, camFar);
  lightView  = glm::lookAt(lightPos,
                                    viewPos,
                                    glm::vec3(0.0f,1.0f,0.0f));
  dirLight.lightSpaceMat = lightProjection * lightView;

  debugDepthQuad.setFloat("near_plane", camNear);
  debugDepthQuad.setFloat("far_plane", camFar);

  dirDepthShader.use();
  dirDepthShader.setMat4("lightSpace",dirLight.lightSpaceMat);
  dirDepthShader.setMat4("model",glm::mat4(1.0f));
  glViewport(0,0,1024,1024);
  glBindFramebuffer(GL_FRAMEBUFFER, dirLight.depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
     glActiveTexture(GL_TEXTURE0);
    drawTerrain(&dirDepthShader,false);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glViewport(0,0,screenWidth,screenHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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


void World::drawPlayers(Shader* shader)
{
  //std::cout << "Map has: " << playerList.size() << "\n";
  for(auto it = playerList.begin(); it != playerList.end();it++)
  {
    it->second->draw(shader);
  }
}

void World::drawFinal()
{
  debugDepthQuad.use();
  glViewport(0,0,200,200);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, dirLight.depthMap);
  renderQuad();
  glViewport(0,0,screenWidth,screenHeight);

  Shader* shader = &blockShader;


  shader->use();
  setLights(shader);
  bindDirectionalShadows(shader);


  viewProj = glm::perspective(glm::radians(45.0f),
                            (float)screenWidth/ (float)screenHeight, 1.0f,
                            (float)(horzRenderDistance-2)*CHUNKSIZE);
  shader->setMat4("projection",viewProj);
  shader->setMat4("view", viewMat);
  shader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
  shader->setVec3("viewPos", viewPos);
  drawTerrain(shader,true);

}

void World::drawTerrain(Shader* shader, bool useHSR)
{
  /*
  Iterates through all the chunks and draws them unless they're marked for destruciton
  then it calls freeGl which will free up all  resourses on the chunk and then
  removes all refrences to it
  At that point the chunk should be deleted by the smart pointers;
  */

  //blockShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

  glm::mat4 hsrProj = glm::perspective(glm::radians(50.0f),
                            (float)screenWidth/ (float)screenHeight, 0.1f,
                            (float)horzRenderDistance*CHUNKSIZE*4);


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

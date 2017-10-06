
#include "headers/all.h"


int WorldWrap::numbOfThreads;
int WorldWrap::seed;
Shader* WorldWrap::blockShader;
Shader* WorldWrap::depthShader;
Shader* WorldWrap::testShader;
int WorldWrap::horzRenderDistance;
int WorldWrap::vertRenderDistance;
GLuint WorldWrap::SHADOW_WIDTH;
GLuint WorldWrap::SHADOW_HEIGHT;
GLuint WorldWrap::VIEW_WIDTH;
GLuint WorldWrap::VIEW_HEIGHT;
unsigned int WorldWrap::totalChunks;
glm::vec3 WorldWrap::lightPos;
Block** WorldWrap::dictionary;
FastNoise WorldWrap::perlin;
GLuint WorldWrap::glTexture;
std::string WorldWrap::worldName;
unsigned int WorldWrap::screenWidth;
unsigned int WorldWrap::screenHeight;

World::World(int numbBuildThreads)
{
  typedef std::queue<std::shared_ptr<BSPNode>> buildType;
  buildQueue = new buildType[numbBuildThreads];
  numbOfThreads = numbBuildThreads;

  seed = 1737;

  perlin.SetSeed(seed);
  perlin.SetFractalOctaves(8);
  perlin.SetFrequency(0.01);
  perlin.SetFractalLacunarity(8);
  perlin.SetFractalGain(5);
  worldName = "default";

  blockShader = new Shader("../src/shaders/shaderBSP.vs","../src/shaders/shaderBSP.fs");
  depthShader = new Shader("../src/shaders/depthShader.vs","../src/shaders/depthShader.fs");
  const char* texture = "../assets/textures/atlas.png";
  loadDictionary("../assets/dictionary.dat");


      std::experimental::filesystem::create_directory("saves");
      std::experimental::filesystem::create_directory("saves/"+worldName);
      std::experimental::filesystem::create_directory("saves/"+worldName+"/chunks");

  frontNode = NULL;
  horzRenderDistance = 15;
  vertRenderDistance = 15;
  totalChunks = 0;
  glGenTextures(1, &glTexture);
  glBindTexture(GL_TEXTURE_2D, glTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //Load and bind the texture from the class
  int texWidth, texHeight;
  unsigned char* image = SOIL_load_image(texture, &texWidth, &texHeight, 0 , SOIL_LOAD_RGBA);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,0,GL_RGBA, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &depthMapFBO);

  int shadowSize = 1024*4;
  SHADOW_WIDTH = shadowSize;
  SHADOW_HEIGHT = shadowSize;
  VIEW_WIDTH = 1920;
  VIEW_HEIGHT = 1080;

  //lightPos = glm::vec3(100.0f,200.0f,100.0f);

  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
               SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  depthShader->use();
  depthShader->setInt("textureInfo", 0);
  depthShader->setInt("shadowMap", 1);
}

void World::destroyWorld()
{
  delete blockShader;

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

  std::shared_ptr<BSPNode>  tempChunk(new BSPNode(chunkx,chunky,chunkz));
  BSPmap[chunkx][chunky][chunkz] = tempChunk;

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
         generateChunk(xchunk+i,ychunk,zchunk);
       }

       zchunk = z-curDis;
       xchunk = x;
       for(int i = -curDis+1;i<curDis;i++)
       {
         generateChunk(xchunk+i,ychunk,zchunk);
       }
       zchunk = z;
       xchunk = x+curDis;
       for(int i = -curDis;i<=curDis;i++)
       {
         generateChunk(xchunk,ychunk,zchunk+i);
       }
       zchunk = z;
       xchunk = x-curDis;
       for(int i = -curDis;i<=curDis;i++)
       {
         generateChunk(xchunk,ychunk,zchunk+i);
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
  if(BSPmap.count(x) == 1)
   {
     if(BSPmap[x].count(y) == 1)
     {
       if(BSPmap[x][y].count(z) == 1)
       {
         return BSPmap[x][y][z];
       }
     }
   }
  return NULL;
}


void World::drawWorld(Camera* camera)
{
  /*
  Iterates through all the chunks and draws them unless they're marked for destruciton
  then it calls freeGl which will free up all opengl resourses on the chunk and then
  removes all refrences to it
  At that point the chunk should be deleted by the smart pointers;
  */

  lightPos = glm::vec3(0.0f,5.0f,0.0f);
  //std::cout << lightPos.x << ":" << lightPos.y<< ":" << lightPos.z << "\n";
  glm::mat4 model = glm::mat4();
  /*
  glm::mat4 lightProjection,lightView,lightSpaceMatrix;
  float orthoSize = 128;
  lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize,orthoSize,1.0f, 128.0f);
  lightView  = glm::lookAt(lightPos,
                                    glm::vec3(8.0f,0.0f,8.0f),
                                    glm::vec3( 0.0f,1.0f,0.0f));


  lightSpaceMatrix = lightProjection * lightView;
  depthShader->use();
  depthShader->setMat4("lightSpaceMatrix",lightSpaceMatrix);
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

      glClear(GL_DEPTH_BUFFER_BIT);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, glTexture);
      depthShader->setMat4("model",model);
      drawOpaque(camera);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
*/

  blockShader->use();

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920/ (float)1080, 0.1f, (float)horzRenderDistance*CHUNKSIZE*4);
  blockShader->setMat4("projection", projection);

  glm::mat4 view = camera->getViewMatrix();
  blockShader->setMat4("view", view);

  blockShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
  blockShader->setVec3("lightColor",  0.5f, 0.5f, 0.5f);
  blockShader->setVec3("lightPos",  lightPos);
  blockShader->setVec3("viewPos", camera->position);
  //blockShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
  blockShader->setMat4("model",model);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, glTexture);
  drawOpaque(camera);
  //drawTranslucent(camera);
}

void World::drawOpaque(Camera* camera)
{
  std::shared_ptr<BSPNode>  curNode = frontNode;
  std::shared_ptr<BSPNode>  nextNode;
  while(curNode != NULL)
  {
    nextNode = curNode->nextNode;
    if(curNode->toDelete == true)
    {
      if(curNode->nextNode != NULL) curNode->nextNode->prevNode = curNode->prevNode;
      if(curNode->prevNode != NULL) curNode->prevNode->nextNode = curNode->nextNode;
      else frontNode = curNode->nextNode;

      curNode->nextNode = NULL;
      curNode->prevNode = NULL;
      curNode->curBSP.freeGL();
    }
    else curNode->drawOpaque(camera);
    curNode = nextNode;
  }
}

void World::drawTranslucent(Camera* camera)
{
  std::shared_ptr<BSPNode> curNode = frontNode;
  std::shared_ptr<BSPNode> nextNode;
  while(curNode != NULL)
  {
    nextNode = curNode->nextNode;
    if(curNode->toDelete == true)
    {
      if(curNode->nextNode != NULL) curNode->nextNode->prevNode = curNode->prevNode;
      if(curNode->prevNode != NULL) curNode->prevNode->nextNode = curNode->nextNode;
      else frontNode = curNode->nextNode;

      curNode->nextNode = NULL;
      curNode->prevNode = NULL;
      curNode->curBSP.freeGL();
    }
    else curNode->drawTranslucent(camera);
    curNode = nextNode;
  }
}


void World::delChunk(int x, int y, int z)
{
  std::shared_ptr<BSPNode>  tempChunk = getChunk(x,y,z);
  if(tempChunk != NULL)
  {
    BSPmap[x][y].erase(z);
    if(BSPmap[x][y].empty())
    {
      BSPmap[x].erase(y);
      if(BSPmap[x].empty())
      {
        BSPmap.erase(x);
      }
    }

    /*
    Essentially marks the chunk for death
    Removes all connected pointers besides the linked list
    The Node however must be destroyed in the draw string
    since that is the sole string using opengl functions;
    othereise the vao wont get freed causing a memory leak
    */
    tempChunk->toDelete = true;
    if(tempChunk->leftChunk != NULL) tempChunk->leftChunk->rightChunk = NULL;
    if(tempChunk->rightChunk != NULL) tempChunk->rightChunk->leftChunk = NULL;
    if(tempChunk->frontChunk != NULL) tempChunk->frontChunk->backChunk = NULL;
    if(tempChunk->backChunk != NULL) tempChunk->backChunk->frontChunk = NULL;
    if(tempChunk->topChunk != NULL) tempChunk->topChunk->bottomChunk = NULL;
    if(tempChunk->bottomChunk != NULL) tempChunk->bottomChunk->topChunk = NULL;

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
    if(abs(chunkx-x) > horzRenderDistance + 1
    || abs(chunky-y) > horzRenderDistance + 1
    || abs(chunkz-z) > horzRenderDistance + 1)
    {
      delChunk(chunkx,chunky,chunkz);
    }
  }
}

bool World::chunkExists(int x ,int y, int z)
{
  if(BSPmap.count(x) == 1)
   {
     if(BSPmap[x].count(y) == 1)
     {
       if(BSPmap[x][y].count(z) == 1)
       {
         return true;
       }
     }
   }
  return false;
}

bool World::blockExists(int x, int y, int z)
{
  /*
  Finds which ever chunk holds the block and then calls the blockExists
  function on that chunk with the localized coordinates
  */
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE)xlocal = 0;
  if(ylocal == CHUNKSIZE)ylocal = 0;
  if(zlocal == CHUNKSIZE)zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);

  std::shared_ptr<BSPNode>  tempChunk;
  if(tempChunk = getChunk(xchunk,ychunk,zchunk))
  {

    if(tempChunk->blockExists(xlocal,ylocal, zlocal))
    {
      return true;
    }
  }
  return false;
}

Block::Block(std::string newName,int newId, int* array, int newVisibleType, int newWidth,int newHeight,int newAtlasWidth, int newAtlasHeight)
{
  visibleType = newVisibleType;
  name = newName;
  id = newId;
  for(int x=0;x<12;x++)
  {
    texArray[x] = array[x];
  }
  width = newWidth;
  height = newHeight;
  atlasWidth = newAtlasWidth;
  atlasHeight = newAtlasHeight;
}

bool World::loadDictionary(const char* file)
{
  /*
  Loads the dictionary in such a format
  TODO: work on improving and discribing that format


  */
  dictionary = new Block*[5];
  int id = 0;
  using namespace std;
  string line;
  ifstream dictionaryf (file);
  if(dictionaryf.is_open())
  {
    getline(dictionaryf,line);
    int atlasWidth = stoi(line);
    getline(dictionaryf,line);
    int atlasHeight = stoi(line);

    int curBlock=0;
    while(getline(dictionaryf,line))
    {
      string name = line;
      getline(dictionaryf,line);

      int texNumb;
      int texArray[12];
      stringstream ss;
      ss << line;
      for(int x = 0;x<12;x++)
      {
        ss >> texNumb;
        texArray[x] = texNumb;
        if(ss.peek() == ',') ss.ignore();
      }
      getline(dictionaryf,line);
      int visibleType = stoi(line);
      getline(dictionaryf,line);
      int width = stoi(line);
      getline(dictionaryf,line);
      int height = stoi(line);
      dictionary[curBlock] = new Block(name,id++,texArray,visibleType,width,height,atlasWidth,atlasHeight);
      curBlock++;
    }
  }
}

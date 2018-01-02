
#include "headers/all.h"

int WorldWrap::screenWidth;
int WorldWrap::screenHeight;
int WorldWrap::numbOfThreads;
int WorldWrap::seed;
int WorldWrap::horzRenderDistance;
int WorldWrap::vertRenderDistance;
int WorldWrap::renderBuffer;
unsigned int WorldWrap::totalChunks;
Block* WorldWrap::dictionary;
FastNoise WorldWrap::perlin;
std::string WorldWrap::worldName;
std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::shared_ptr<BSPNode>>>> WorldWrap::BSPmap;


World::World(int numbBuildThreads,int width,int height)
{
  typedef std::queue<std::shared_ptr<BSPNode>> buildType;
  buildQueue = new buildType[numbBuildThreads];
  numbOfThreads = numbBuildThreads;

  seed = 1737;
  screenWidth = width;
  screenHeight = height;

  perlin.SetSeed(seed);
  perlin.SetFractalOctaves(8);
  perlin.SetFrequency(0.01);
  perlin.SetFractalLacunarity(8);
  perlin.SetFractalGain(5);
  worldName = "default";

  const char* texture = "../assets/textures/atlas.png";
  loadDictionary("../assets/blockDictionary.dat");

/*
  std::experimental::filesystem::create_directory("saves");
  std::experimental::filesystem::create_directory("saves/"+worldName);
  std::experimental::filesystem::create_directory("saves/"+worldName+"/chunks");
*/

  frontNode = NULL;
  horzRenderDistance = 7;
  vertRenderDistance = 7;
  renderBuffer = 1;
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

  /*
  glGenFramebuffers(1, &depthMapFBO);


  int shadowSize = 10240;
  SHADOW_WIDTH = shadowSize;
  SHADOW_HEIGHT = shadowSize;
  VIEW_WIDTH = width;
  VIEW_HEIGHT = height;

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

  blockShader.use();
  blockShader.setInt("curTexture", 0);
  blockShader.setInt("shadowMap", 1);
  debugShader.use();
  debugShader.setInt("depthMap",0);
  */
}

void World::renderQuad()
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

void World::drawWorld()
{
  /*
  Iterates through all the chunks and draws them unless they're marked for destruciton
  then it calls freeGl which will free up all opengl resourses on the chunk and then
  removes all refrences to it
  At that point the chunk should be deleted by the smart pointers;
  */

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, glTexture);
  drawOpaque();

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


std::shared_ptr<BSPNode>  WorldWrap::getChunk(int x, int y, int z)
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



void World::drawOpaque()
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
    else curNode->drawOpaque();
    curNode = nextNode;
  }
}

void World::drawTranslucent()
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
    else curNode->drawTranslucent();
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

//If there is an entity, returns it id and position,
//If there is a block, return its position and an id of 0
//If there is nothing return a 0 vector  with -1 id
glm::vec4 WorldWrap::rayCast(glm::vec3 pos, glm::vec3 front, int max)
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

bool WorldWrap::blockExists(int x, int y, int z)
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
void World::addBlock(int x, int y, int z, int id)
{
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE) xlocal = 0;
  if(ylocal == CHUNKSIZE) ylocal = 0;
  if(zlocal == CHUNKSIZE) zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);

  std::shared_ptr<BSPNode>  tempChunk;
  if(tempChunk = getChunk(xchunk,ychunk,zchunk))
  {
    tempChunk->addBlock(xlocal,ylocal,zlocal,id);
  }
  tempChunk->build();

  if(xlocal+1>=CHUNKSIZE)
  {
    getChunk(xchunk+1,ychunk,zchunk)->build();
  }
  else if(xlocal-1 < 0)
  {
    getChunk(xchunk-1,ychunk,zchunk)->build();
  }

  if(ylocal+1>=CHUNKSIZE)
  {
    getChunk(xchunk,ychunk+1,zchunk)->build();
  }
  else if(ylocal-1 < 0)
  {
    getChunk(xchunk,ychunk-1,zchunk)->build();
  }

  if(zlocal+1>=CHUNKSIZE)
  {
    getChunk(xchunk,ychunk,zchunk+1)->build();
  }
  else if(zlocal-1 < 0)
  {
    getChunk(xchunk,ychunk,zchunk-1)->build();
  }


}

void World::delBlock(int x, int y, int z)
{
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE) xlocal = 0;
  if(ylocal == CHUNKSIZE) ylocal = 0;
  if(zlocal == CHUNKSIZE) zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);

  std::shared_ptr<BSPNode>  tempChunk;
  if(tempChunk = getChunk(xchunk,ychunk,zchunk))
  {
    tempChunk->delBlock(xlocal,ylocal,zlocal);
  }
  tempChunk->build();

  if(xlocal+1>=CHUNKSIZE)
  {
    getChunk(xchunk+1,ychunk,zchunk)->build();
  }
  else if(xlocal-1 < 0)
  {
    getChunk(xchunk-1,ychunk,zchunk)->build();
  }

  if(ylocal+1>=CHUNKSIZE)
  {
    getChunk(xchunk,ychunk+1,zchunk)->build();
  }
  else if(ylocal-1 < 0)
  {
    getChunk(xchunk,ychunk-1,zchunk)->build();
  }

  if(zlocal+1>=CHUNKSIZE)
  {
    getChunk(xchunk,ychunk,zchunk+1)->build();
  }
  else if(zlocal-1 < 0)
  {
    getChunk(xchunk,ychunk,zchunk-1)->build();
  }
}

void World::updateBlock(int x, int y, int z)
{
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE) xlocal = 0;
  if(ylocal == CHUNKSIZE) ylocal = 0;
  if(zlocal == CHUNKSIZE) zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);
  std::shared_ptr<BSPNode>  tempChunk = getChunk(xchunk,ychunk,zchunk);
  if(!tempChunk->toBuild)
  {
    addToBuildQueue(tempChunk);
  }

}


int WorldWrap::anyExists(glm::vec3 pos)
{
  if(entityExists(pos.x,pos.y,pos.z))
    return 2;
  else if( blockExists(pos.x,pos.y,pos.z))
    return 1;
  else return 0;

}

bool WorldWrap::entityExists(glm::vec3 pos)
{
  //TODO
  return false;
}
bool WorldWrap::entityExists(float x, float y, float z)
{
  //TODO
  return false;
}

Block::Block(std::string newName,int newId, int* array, int newVisibleType, int newWidth,int newHeight,int newDepth, int newAtlasWidth, int newAtlasHeight)
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
  depth = newDepth;
  atlasWidth = newAtlasWidth;
  atlasHeight = newAtlasHeight;
}

bool World::loadDictionary(const char* file)
{
  /*
  Loads the dictionary in such a format
  TODO: work on improving and discribing that format


  */
  dictionary = new Block[256];
  int id = 0;
  using namespace std;
  string line;
  ifstream dictionaryf (file);

  if(dictionaryf.is_open())
  {
    std::cout << "Loading dictionary\n";
    getline(dictionaryf,line);
    int atlasWidth = stoi(line);
    getline(dictionaryf,line);
    int atlasHeight = stoi(line);

    int curBlock=0;
    while(getline(dictionaryf,line))
    {
      if(line != "{") continue;
      getline(dictionaryf,line);
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
      getline(dictionaryf,line);
      int depth = stoi(line);
      dictionary[curBlock] = Block(name,id++,texArray,visibleType,width,height,depth,atlasWidth,atlasHeight);
      curBlock++;
    }
    return true;
  }
  else
  {
    std::cout <<"ERROR: block dictionary not found\n";
    return false;
  }
}

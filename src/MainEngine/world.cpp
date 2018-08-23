
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include "include/world.h"
#include "include/bsp.h"
#include "../Settings/settings.h"


#include "../TextureLoading/textureloading.h"

#include "include/messenger.h"
#include "../Objects/include/items.h"
//#include "worldDrawingEngine.h"


World::World(int numbBuildThreads,int width,int height)
{
  buildQueue = new TSafeQueue<std::shared_ptr<BSPNode>>[numbBuildThreads];
  numbOfThreads = numbBuildThreads;

  ItemDatabase::loadBlockDictionary("../assets/blockDictionary.dat");
  ItemDatabase::loadItemDictionary("../assets/itemDictionary.dat");
  //Settings::set("unlockCamera","0");
  Settings::print();
  try
  {
    horzRenderDistance = std::stoi(Settings::get("horzRenderDistance"));
    vertRenderDistance = std::stoi(Settings::get("vertRenderDistance"));
    renderBuffer = std::stoi(Settings::get("renderBuffer"));
    drawer.directionalShadowResolution = std::stoi(Settings::get("dirShadowResolution"));
  }
  catch(...)
  {
    std::ofstream error("errorlog.txt");
    error << "Bad settings\n";
  }
  worldName = Settings::get("worldName");
  std::string ipAddress = Settings::get("ipAddress");
  std::string port = Settings::get("port");
  std::string userName = Settings::get("userName");
  boost::filesystem::create_directory("saves");
  boost::filesystem::create_directory("saves/"+worldName);
  boost::filesystem::create_directory("saves/"+worldName+"/chunks");

  drawnChunks = 0;

  try
  {
    messenger.setupSockets(ipAddress,port);
    messenger.sendMessage(userName.c_str(),24);
    messenger.receiveMessage(&mainId,sizeof(mainId));
  }catch(const char* err)
  {
    std::ofstream error("errorlog.txt");
    error << err;
  }
  std::cout << "Done\n";


  drawer.setRenderDistances(vertRenderDistance,horzRenderDistance,renderBuffer);
  drawer.setupShadersAndTextures(width,height,&BSPmap);
  drawer.updateViewProjection(45.0f,0.1f,(horzRenderDistance)*CHUNKSIZE);
}



void World::movePlayer(const glm::vec3 &pos, uchar id)
{
  if(drawer.playerList.count(id) != 1)
  {
    std::cout << "ERROR: Attempting to move player that doesnt exist\n";
    return;
  }
  std::shared_ptr<Object> tmp = drawer.playerList[id];
  if(tmp != NULL)
  {
    tmp->setPosition(pos);
    tmp->updateModelMat();
  }

}


void World::addPlayer(const glm::vec3 &pos, uchar id)
{
  std::cout << "Adding player at" << glm::to_string(pos) << ":"<< (int) id << "\n";
  std::shared_ptr<Player> temp(new Player(pos));
  drawer.playerList[id] = temp;
}


void World::removePlayer(uchar id)
{

  std::cout << "Removing PLayer\n";
  drawer.playerList.erase(id);

}

std::shared_ptr<BSPNode> World::chunkRayCast(const glm::vec3 &pos, const glm::vec3 &front)
{
  for(int i =0;i<horzRenderDistance;i++)
  {
    glm::vec3 curPos = pos + front*float(i*CHUNKSIZE);
    glm::ivec3 chunkPos = toChunkCoords(curPos);
    glm::ivec3 localPos = toLocalCoords(curPos);
    auto chunk = getChunk(chunkPos);
    if(chunk!=NULL)
    {
      if(chunk->blockExists(chunkPos))
      {
        return chunk;
      }
    }
  }
  return NULL;
}

void World::calculateViewableChunks()
{
  glm::ivec3 min = toChunkCoords(drawer.viewMin);
  glm::ivec3 max = toChunkCoords(drawer.viewMax);
  drawer.chunksToDraw = BSPmap.findAll(min,max);
}


void World::generateChunkFromString(const glm::ivec3 &chunk,const char* value)
{
  //std::cout << "Generating chunk" << chunkx << ":" << chunky << ":" << chunkz << "\n";
  if(chunkExists(chunk))
  {
    std::cout << "Replacing CHunk \n";
    delChunk(chunk);
  }
  std::shared_ptr<BSPNode> tempChunk(new BSPNode(glm::ivec3(chunk),value));
  BSPmap.add(glm::ivec3(chunk),tempChunk);

  int chunkx = chunk.x;
  int chunky = chunk.y;
  int chunkz = chunk.z;
  /*
    At this point the chunk is in both the linked list as well as the unorderedmap
    now it will attempt to find any nearby chunks and store a refrence to it
  */


  std::shared_ptr<BSPNode>  otherChunk = getChunk(glm::ivec3(chunkx,chunky,chunkz-1));
  if(otherChunk  != NULL )
  {
    tempChunk->frontChunk = otherChunk;
    otherChunk->backChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(glm::ivec3(chunkx,chunky,chunkz+1));
  if(otherChunk != NULL)
  {
    tempChunk->backChunk = otherChunk;
    otherChunk->frontChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(glm::ivec3(chunkx,chunky-1,chunkz));
  if(otherChunk != NULL)
  {
    tempChunk->bottomChunk = otherChunk;
    otherChunk->topChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(glm::ivec3(chunkx,chunky+1,chunkz));
  if(otherChunk != NULL)
  {
    tempChunk->topChunk = otherChunk;
    otherChunk->bottomChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(glm::ivec3(chunkx-1,chunky,chunkz));
  if(otherChunk != NULL)
  {
    tempChunk->leftChunk = otherChunk;
    otherChunk->rightChunk = tempChunk;
    if(!otherChunk->toBuild)
    {
      addToBuildQueue(otherChunk);
    }
  }

  otherChunk = getChunk(glm::ivec3(chunkx+1,chunky,chunkz));
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


void World::findChunkToRequest(const float mainx,const float mainy,const float mainz)
{
  int radius,x,y,z;
  radius = 0;
  glm::ivec3 org = toChunkCoords(glm::ivec3(mainx,mainy,mainz));
  std::cout << "Starting search\n";
  x = 0;
  y = 0;
  z = 0;
  while(radius <= horzRenderDistance)
  {

    //std::cout << "Checking for chunk at" << org.x+x << ":"<< org.y+y<< ":" << org.z+z<< "\n";
    if(!chunkExists(glm::ivec3(org.x+x,org.y+y,org.z+z)))
    {
      std::cout << "Found missing chunk\n";
      messenger.createChunkRequest(org.x+x,org.y+y,org.z+z);
      return;
    }
    if(x < radius)
    {
      x++;
    }
    else if(y < radius)
    {
      y++;
    }
    else if(z < radius)
    {
      z++;
    }
    else
    {
      radius++;
      x = 0;
      y = 0;
      z = 0;
    }

  }

}
void World::renderWorld(float mainx, float mainy, float mainz)
{
  double lastFrame = 0;
  double currentFrame = 0;
  double ticksPerSecond = 500;
  double tickRate = 1.0f/ticksPerSecond;
  for(int x =0;x<horzRenderDistance*2;x++)
  {
    for(int z=0;z<horzRenderDistance*2;z++)
    {
      for(int y=0;y<vertRenderDistance*2;y++)
      {
        int horzSquared = horzRenderDistance*horzRenderDistance;
        int vertSquared = vertRenderDistance*vertRenderDistance;
        int curx = (x%2 == 0) ? x/2 : -(x/2+1);
        int cury = (y%2 == 0) ? y/2 : -(y/2+1);
        int curz = (z%2 == 0) ? z/2 : -(z/2+1);

        if((curx*curx)/horzSquared + (cury*cury)/vertSquared + (curz*curz)/horzSquared > 1) continue;
        int xchunk = round(mainx);
        int ychunk = round(mainy);
        int zchunk = round(mainz);
        xchunk/= CHUNKSIZE;
        ychunk/= CHUNKSIZE;
        zchunk/= CHUNKSIZE;
        if(!chunkExists(glm::ivec3(xchunk+curx,ychunk+cury,zchunk+curz)))
        {
          messenger.createChunkRequest(xchunk+curx,ychunk+cury,zchunk+curz);
          lastFrame = currentFrame;
          currentFrame = glfwGetTime();
          double deltaFrame = currentFrame-lastFrame;
          int waitTime = (tickRate-deltaFrame)*1000;
          std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
          currentFrame = glfwGetTime();
        }
      }
    }
  }
}

void World::buildWorld(int threadNumb)
{
  while(!buildQueue[threadNumb].empty())
  {
    std::shared_ptr<BSPNode> chunk = buildQueue[threadNumb].front();
    chunk->build();
    buildQueue[threadNumb].pop();
  }
}

std::shared_ptr<BSPNode>  World::getChunk(const glm::ivec3 &pos)
{
  return BSPmap.get(pos);
}

void World::deleteChunksFromQueue()
{
  while(!chunkDeleteQueue.empty())
  {
    auto temp = chunkDeleteQueue.front();
    temp->del();
    chunkDeleteQueue.pop();
  }
}

void World::delChunk(const glm::ivec3 &pos)
{
  std::shared_ptr<BSPNode>  tempChunk = getChunk(pos);
  if(tempChunk != NULL)
  {
    BSPmap.del(pos);
    messenger.requestMap.del(pos);
    tempChunk->disconnect();
    chunkDeleteQueue.push(tempChunk);
    /*
    Essentially marks the chunk for death
    Removes all connected pointers besides the linked list
    The Node however must be destroyed in the draw string
    since that is the sole string using opengl functions;
    othereise the vao wont get freed causing a memory leak
    */
  }
}


void World::delScan(float mainx, float mainy, float mainz)
{
  /*
    Scans through the same list as the draw function
    however this time referencing the current position of the player
    and raises the destroy flag if its out of a certain range
    */

  auto list = BSPmap.getFullList();
  for(auto itr = list.begin();itr != list.end();++itr)
  {
    int x = round(mainx);
    int y = round(mainy);
    int z = round(mainz);

    x/= CHUNKSIZE;
    y/= CHUNKSIZE;
    z/= CHUNKSIZE;

    glm::ivec3 chunk = (*itr)->curBSP.chunkPos;

    if(abs(chunk.x-x) > horzRenderDistance + renderBuffer
    || abs(chunk.y-y) > horzRenderDistance + renderBuffer
    || abs(chunk.z-z) > horzRenderDistance + renderBuffer)
    {
      delChunk(chunk);
    }
  }
}

bool World::chunkExists(const glm::ivec3 &pos)
{
  return BSPmap.exists(pos);
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



glm::ivec3 inline World::toLocalCoords(glm::ivec3 in)
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

glm::ivec3 inline World::toChunkCoords(glm::ivec3 in)
{
  glm::ivec3 out;
  out.x = floor((float)in.x/(float)CHUNKSIZE);
  out.y = floor((float)in.y/(float)CHUNKSIZE);
  out.z = floor((float)in.z/(float)CHUNKSIZE);
  return out;
}

inline void World::checkForUpdates(glm::ivec3 local,std::shared_ptr<BSPNode> chunk)
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

bool World::blockExists(const glm::ivec3 &pos)
{
  /*
  Finds which ever chunk holds the block and then calls the blockExists
  function on that chunk with the localized coordinates
  */
  glm::ivec3 local = toLocalCoords(pos);
  glm::ivec3 chunk = toChunkCoords(pos);
  std::shared_ptr<BSPNode>  tempChunk = getChunk(chunk);
  if(tempChunk != NULL)
  {

    if(tempChunk->blockExists(local))
    {
      return true;
    }
  }
  return false;
}

void World::addBlock(const glm::ivec3 &pos,uchar id)
{
  glm::ivec3 local = toLocalCoords(pos);
  glm::ivec3 chunk = toChunkCoords(pos);

  std::shared_ptr<BSPNode> tempChunk = getChunk(chunk);
  if(tempChunk != NULL)
  {
    tempChunk->addBlock(local,id);
    tempChunk->build();
    checkForUpdates(local,tempChunk);
  }

}


void World::delBlock(const glm::ivec3 &pos)
{
  glm::ivec3 local = toLocalCoords(pos);
  glm::ivec3 chunk = toChunkCoords(pos);

  std::shared_ptr<BSPNode>  tempChunk = getChunk(chunk);
  if(tempChunk != NULL)
  {
    tempChunk->delBlock(local);
    tempChunk->build();
    checkForUpdates(local,tempChunk);
  }
}


void World::updateBlock(const glm::ivec3 &pos)
{
  glm::ivec3 chunk = toChunkCoords(pos);
  std::shared_ptr<BSPNode>  tempChunk = getChunk(chunk);
  if(!tempChunk->toBuild)
  {
    addToBuildQueue(tempChunk);
  }

}


int World::anyExists(const glm::vec3 &pos)
{
  if(entityExists(pos))
    return 2;
  else if( blockExists(pos))
    return 1;
  else return 0;

}

bool World::entityExists(const glm::vec3 &pos)
{
  //TODO
  return false;
}

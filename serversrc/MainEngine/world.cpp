
#include "include/world.h"
#include "include/bsp.h"

World::World()
{

  worldName = "server";
  boost::filesystem::create_directory("saves");
  boost::filesystem::create_directory("saves/"+worldName);
  boost::filesystem::create_directory("saves/"+worldName+"/chunks");
  totalChunks = 0;
}

  
void World::generateChunk(int chunkx, int chunky, int chunkz)
{
  if(chunkExists(chunkx,chunky,chunkz)) return;

  std::shared_ptr<BSPNode>  tempChunk(new BSPNode(chunkx,chunky,chunkz,worldName));
  BSPmap.add(chunkx,chunky,chunkz,tempChunk);
  /*
    At this point the chunk is in both the linked list as well as the unorderedmap
    now it will attempt to find any nearby chunks and store a refrence to it
  */
  std::shared_ptr<BSPNode>  otherChunk = getChunk(chunkx,chunky,chunkz-1);
  if(otherChunk  != NULL )
  {
    tempChunk->frontChunk = otherChunk;
    otherChunk->backChunk = tempChunk;
  }

  otherChunk = getChunk(chunkx,chunky,chunkz+1);
  if(otherChunk != NULL)
  {
    tempChunk->backChunk = otherChunk;
    otherChunk->frontChunk = tempChunk;
  }

  otherChunk = getChunk(chunkx,chunky-1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->bottomChunk = otherChunk;
    otherChunk->topChunk = tempChunk;
  }

  otherChunk = getChunk(chunkx,chunky+1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->topChunk = otherChunk;
    otherChunk->bottomChunk = tempChunk;
  }

  otherChunk = getChunk(chunkx-1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->leftChunk = otherChunk;
    otherChunk->rightChunk = tempChunk;
  }

  otherChunk = getChunk(chunkx+1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->rightChunk = otherChunk;
    otherChunk->leftChunk = tempChunk;
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
    if(tempChunk->leftChunk != NULL) tempChunk->leftChunk->rightChunk = NULL;
    if(tempChunk->rightChunk != NULL) tempChunk->rightChunk->leftChunk = NULL;
    if(tempChunk->frontChunk != NULL) tempChunk->frontChunk->backChunk = NULL;
    if(tempChunk->backChunk != NULL) tempChunk->backChunk->frontChunk = NULL;
    if(tempChunk->topChunk != NULL) tempChunk->topChunk->bottomChunk = NULL;
    if(tempChunk->bottomChunk != NULL) tempChunk->bottomChunk->topChunk = NULL;

  }
}


void World::saveWorld()
{
  std::cout << "saving World\n";
  auto list = BSPmap.getFullList();
  for(auto itr = list.begin();itr != list.end();++itr)
  {
    (*itr)->saveChunk();
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

    if(tempChunk->blockExists(local.x,local.y,local.z))
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
  }
}


void World::updateBlock(int x, int y, int z)
{
  glm::ivec3 pos(x,y,z);
  glm::ivec3 chunk = toChunkCoords(pos);
  std::shared_ptr<BSPNode>  tempChunk = getChunk(chunk.x,chunk.y,chunk.z);

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

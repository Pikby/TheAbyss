
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <atomic>
#include <list>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#include "../headers/shaders.h"
// GLFW
#include <GLFW/glfw3.h>
#include "include/bsp.h"
#include "../Objects/include/items.h"

int BSPNode::totalChunks = 0;
bool BSP::geometryChanged = true;
BSPNode::~BSPNode()
{
  totalChunks--;
}

BSPNode::BSPNode(const glm::ivec3 &pos,const char *val) : curBSP(val,pos,this),chunkPos(pos)
{
  totalChunks++;
}

void BSPNode::build()
{
  if(toDelete) return;
  BSPMutex.lock();
    curBSP.build();
    toRender = true;
    toBuild = false;
  BSPMutex.unlock();
}

void BSPNode::drawOpaque(Shader* shader, const glm::vec3 &pos)
{
  if(toDelete) return;
  if(toRender == true)
  {
    if(BSPMutex.try_lock())
    {
      //curBSP.swapBuffers();
      curBSP.render();
      toRender = false;
      BSPMutex.unlock();
    }
  }
  curBSP.drawOpaque(shader,pos);
}

void BSPNode::drawTranslucent(Shader* shader, const glm::vec3 &pos)
{
  curBSP.drawTranslucent(shader,pos);
}
std::list<Light> BSPNode::getFromLightList(int count)
{
  return curBSP.getFromLightList(count);
}

void BSPNode::del()
{
  BSPMutex.lock();
    curBSP.freeGL();
  BSPMutex.unlock();
}

void BSPNode::disconnect()
{
  BSPMutex.lock();
  toDelete = true;
  if(leftChunk != NULL) leftChunk->rightChunk = NULL;
  if(rightChunk != NULL) rightChunk->leftChunk = NULL;
  if(frontChunk != NULL) frontChunk->backChunk = NULL;
  if(backChunk != NULL) backChunk->frontChunk = NULL;
  if(topChunk != NULL) topChunk->bottomChunk = NULL;
  if(bottomChunk != NULL) bottomChunk->topChunk = NULL;
  leftChunk = NULL;
  rightChunk = NULL;
  frontChunk = NULL;
  backChunk = NULL;
  topChunk = NULL;
  bottomChunk = NULL;
  BSPMutex.unlock();

}

uint8_t BSPNode::getBlockOOB(const glm::ivec3 &pos)
{
  //std::lock_guard<std::mutex> lock(BSPMutex);
  auto check = [&](std::shared_ptr<BSPNode> chunk,const glm::ivec3 &norm)
  {
    return chunk != NULL ? chunk->getBlockOOB(pos+CHUNKSIZE*norm) : 0;
  };
  if(pos.x >= CHUNKSIZE)
  {
    return check(rightChunk,glm::ivec3(-1,0,0));
  }
  else if(pos.x < 0)
  {
    return check(leftChunk,glm::ivec3(1,0,0));
  }
  else if(pos.y >= CHUNKSIZE)
  {
    return check(topChunk,glm::ivec3(0,-1,0));
  }
  else if(pos.y < 0)
  {
    return check(bottomChunk,glm::ivec3(0,1,0));
  }
  else if(pos.z >= CHUNKSIZE)
  {
    return check(backChunk,glm::ivec3(0,0,-1));
  }
  else if(pos.z < 0)
  {
    return check(frontChunk,glm::ivec3(0,0,1));
  }
  else return getBlock(pos);
}

bool BSPNode::blockExists(const glm::ivec3 &pos)
{
  std::lock_guard<std::recursive_mutex> lock(BSPMutex);
  return curBSP.blockExists(pos);
}

void BSPNode::delBlock(const glm::ivec3 &pos)
{
  std::lock_guard<std::recursive_mutex> lock(BSPMutex);
  curBSP.delBlock(pos);
}

void BSPNode::addBlock(const glm::ivec3 &pos, uint8_t id)
{
  std::lock_guard<std::recursive_mutex> lock(BSPMutex);
  curBSP.addBlock(pos,id);
}

uint8_t BSPNode::getBlock(const glm::ivec3 &pos)
{
  return curBSP.getBlock(pos);
}

RenderType BSPNode::blockVisibleType(const glm::ivec3 &pos)
{
  std::lock_guard<std::recursive_mutex> lock(BSPMutex);
  return curBSP.blockVisibleType(pos);
}

std::shared_ptr<BSPNode> BSPNode::getNeighbour(Faces face)
{
  std::lock_guard<std::recursive_mutex> lock(BSPMutex);
  switch(face)
  {
    case TOPF:
      return topChunk;
    case BOTTOMF:
      return bottomChunk;
    case LEFTF:
      return leftChunk;
    case RIGHTF:
      return rightChunk;
    case FRONTF:
      return frontChunk;
    case BACKF:
      return backChunk;
    default:
      std::cout << "ERROR INVALID ENUM \n";
      return topChunk;
  }
}

void BSPNode::setNeighbour(Faces face, std::shared_ptr<BSPNode> neighbour)
{
  std::lock_guard<std::recursive_mutex> lock(BSPMutex);
  switch(face)
  {
    case TOPF:
      topChunk = neighbour; break;
    case BOTTOMF:
      bottomChunk = neighbour; break;
    case LEFTF:
      leftChunk = neighbour; break;
    case RIGHTF:
      rightChunk = neighbour; break;
    case FRONTF:
      frontChunk = neighbour; break;
    case BACKF:
      backChunk = neighbour; break;
  }
}


glm::ivec3 BSPNode::getRealWorldPosition()
{
  return (CHUNKSIZE*chunkPos) + glm::ivec3(CHUNKSIZE/2);
}

BSP::BSP(const char* data,const glm::ivec3 &pos,BSPNode* Parent) : parent(Parent)
{
  //blockOrigin = chunkLocalPos + CHUNKSIZE*(parent->chunkPos);
  modelMat = glm::translate(glm::mat4(1.0f),glm::vec3(CHUNKSIZE*pos));

  using namespace std;
  const int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
  int i = 0;
  char curId = 0;
  unsigned short curLength = 0;
  unsigned int index=0;

  while(i<numbOfBlocks)
  {
    curId = data[index];
    curLength = 0;
    index++;
    for(int j=0;j<sizeof(curLength);j++)
    {
      curLength += ((uint8_t) data[index] << (j*8));
      index++;
    }
    for(int j = 0; j<curLength; j++)
    {
      if(i+j>numbOfBlocks)
      {
        std::cout << "ERROR CORRUPTED CHUNK AT " << glm::to_string(parent->getPosition()) <<"\n";
        delete[] data;
        return;
      }
      worldArray[i+j] = curId;
    }
    i+= curLength;
  }
  delete[] data;

}


void BSP::freeGL()
{
  //Frees all the used opengl resourses
  //MUST BE DONE IN THE MAIN THHREAD
  //Since that is the only thread with opengl context
  glDeleteVertexArrays(1,&oVAO);
  glDeleteBuffers(1,&oVBO);
  glDeleteBuffers(1,&oEBO);


  glDeleteBuffers(1,&tVBO);
  glDeleteBuffers(1,&tEBO);
  glDeleteVertexArrays(1,&tVAO);

}



bool BSP::blockExists(const glm::ivec3 &pos)
{
  return (worldArray.get(pos) != 0);
}

RenderType BSP::blockVisibleType(const glm::ivec3 &pos)
{
  return ItemDatabase::blockDictionary[getBlock(pos)].visibleType;
}

void BSP::addBlock(const glm::ivec3 &pos, char id)
{
  worldArray.set(pos,id);
}

void BSP::addToLightList(const glm::ivec3 &localPos,const Light& light)
{
  lightList[localPos.x*CHUNKSIZE*CHUNKSIZE+localPos.z*CHUNKSIZE+localPos.y] = light;
}

bool BSP::lightExists(const glm::ivec3 &localPos)
{
  return lightList.count(localPos.x*CHUNKSIZE*CHUNKSIZE+localPos.z*CHUNKSIZE+localPos.y);
}
void BSP::removeFromLightList(const glm::ivec3 &localPos)
{
  std::cout << "Erasing from map\n";
  lightList.erase(localPos.x*CHUNKSIZE*CHUNKSIZE+localPos.z*CHUNKSIZE+localPos.y);
}

std::list<Light> BSP::getFromLightList(int count)
{
  std::list<Light> list;
  int counter = 0;
  for(auto iter = lightList.begin();iter != lightList.end();iter++)
  {
    list.push_back(iter->second);
    counter++;
    if(counter + 1 >= count) break;
  }

  return list;
}

inline void BSP::delBlock(const glm::ivec3 &pos)
{
  worldArray.set(pos,0);
}

uint8_t BSP::getBlock(const glm::ivec3 &pos)
{
  return worldArray.get(pos);
}



void BSP::setupBufferObjects(RenderType type)
{
  std::vector<float>* curVert;
  std::vector<uint>* curInd;
  uint *VBO;
  uint *EBO;
  uint *VAO;
  int *indicesSize;
  if(type == OPAQUE)
  {
    indicesSize = &oIndicesSize;
    curVert = &oVertices;
    curInd = &oIndices;
    VBO = &oVBO;
    EBO = &oEBO;
    VAO = &oVAO;
  }
  else if(type == TRANSLUCENT)
  {
    indicesSize = &tIndicesSize;
    curVert = &tVertices;
    curInd = &tIndices;
    VBO = &tVBO;
    EBO = &tEBO;
    VAO = &tVAO;
  }
  *indicesSize = curInd->size();
  if(*indicesSize != 0)
  {
    glDeleteBuffers(1, VBO);
    glDeleteBuffers(1, EBO);
    glDeleteVertexArrays(1, VAO);

    glGenVertexArrays(1, VAO);
    glGenBuffers(1, EBO);
    glGenBuffers(1, VBO);
    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO);
    glBufferData(GL_ARRAY_BUFFER, curVert->size()*sizeof(float),&curVert->front(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, curInd->size()*sizeof(uint),&curInd->front(), GL_DYNAMIC_DRAW);

    int vertexSize = 5*sizeof(float);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,vertexSize, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,vertexSize, (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,vertexSize, (void*)(4*sizeof(float)));
    glEnableVertexAttribArray(2);



    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    geometryChanged = true;
    *curVert = std::vector<float>();
    *curInd = std::vector<uint>();
  }
}

void BSP::render()
{
  setupBufferObjects(OPAQUE);
  setupBufferObjects(TRANSLUCENT);
}

void BSP::drawOpaque(Shader* shader, const glm::vec3 &pos)
{
  if(oIndicesSize != 0)
  {
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(CHUNKSIZE*parent->getPosition())-pos);
    //std::cout << glm::to_string(glm::vec3(parent->chunkPos) - pos) << "\n";
    shader->setMat4("model",model);
    glBindVertexArray(oVAO);
    glDrawElements(GL_TRIANGLES, oIndicesSize, GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

  }
}

void BSP::drawTranslucent(Shader* shader,const glm::vec3 &pos)
{
  if(tIndicesSize)
  {
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(CHUNKSIZE*parent->getPosition())-pos);
    shader->setMat4("model",model);
    glBindVertexArray(tVAO);
    glDrawElements(GL_TRIANGLES, tIndicesSize, GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
  }

}

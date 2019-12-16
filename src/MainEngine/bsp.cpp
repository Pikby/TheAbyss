
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
#include "../Objects/include/objects.h"
#include "../Settings/settings.h"
inline int pack4chars(char a, char b, char c, char d)
{
return ((a << 24) | (b << 16) | (c << 8) | d);
}


int BSPNode::totalChunks = 0;
bool BSP::geometryChanged = true;
Shader BSP::tShader, BSP::oShader;

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

void BSPNode::drawPreviewBlock(const glm::ivec3& pos,const glm::vec3& viewPos)
{
  curBSP.drawPreviewBlock(pos,viewPos);
}



void BSPNode::drawOpaque(const glm::vec3 &pos)
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
  curBSP.drawOpaque(pos);
}

void BSPNode::drawTranslucent(const glm::vec3 &pos)
{
  curBSP.drawTranslucent(pos);
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

void BSP::updateMatrices(Camera& camera)
{
  oShader.use();
  oShader.setMat4("projection",camera.getProjectionMatrix());
  oShader.setMat4("view",camera.getViewMatrix());

  tShader.use();
  tShader.setMat4("projection",camera.getProjectionMatrix());
  tShader.setMat4("view",camera.getViewMatrix());
}

void BSP::initializeBSPShader(const glm::vec2& textureAtlasDimensions)
{
  int cellWidth = 128;
  oShader = Shader("BSPShaders/gBuffer.fs","BSPShaders/gBuffer.vs");
  oShader.use();
  oShader.setInt("textureAtlasWidthInCells",textureAtlasDimensions.x/cellWidth);
  oShader.setInt("textureAtlasHeightInCells",textureAtlasDimensions.y/cellWidth);
  oShader.setInt("cellWidth",cellWidth);
  oShader.setInt("textureAtlas",0);
  oShader.setVec3("objectColor", 0.5f, 0.5f, 0.31f);


  tShader = Shader("BSPShaders/transShader.fs","BSPShaders/gBuffer.vs");
  tShader.use();
  tShader.setInt("textureAtlasWidthInCells",textureAtlasDimensions.x/cellWidth);
  tShader.setInt("textureAtlasHeightInCells",textureAtlasDimensions.y/cellWidth);
  tShader.setInt("cellWidth",cellWidth);
  tShader.setInt("textureAtlas",0);
  tShader.setVec3("objectColor", 0.5f, 0.5f, 0.31f);

}

void BSP::setTerrainColor(const glm::vec3& color)
{
  oShader.use();
  oShader.setVec3("objectColor", color);

  tShader.use();
  tShader.setVec3("objectColor",color);
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


void BSP::addIndices(RenderType renderType,int index1, int index2, int index3, int index4)
{
  std::vector<uint>* curBuffer;
  if(renderType == OPAQUE)
  {
    curBuffer = &oIndices;
  }
  else if(renderType == TRANSLUCENT)
  {
    curBuffer = &tIndices;
  }
  else
  {
    std::cout << "Indices error\n";
    return;
  }
  //Add the First triangle of the square
  curBuffer->push_back(index1);
  curBuffer->push_back(index2);
  curBuffer->push_back(index3);

  //Add the second triangle of the square
  curBuffer->push_back(index4);
  curBuffer->push_back(index1);
  curBuffer->push_back(index3);
}


int BSP::addVertex(const VertexData &vertex)
{

  std::vector<float>* curBuffer;
  if(vertex.renderType == OPAQUE)
  {
    curBuffer = &oVertices;
  }
  else if(vertex.renderType == TRANSLUCENT)
  {
    curBuffer = &tVertices;
  }
  else
  {
    std::cout << "adding vertex of a transparentobject ???\n";
  }
  int numbVert = curBuffer->size()/8;
  //Adds position vector
  //curBuffer->push_back(*(float*)&fullPos);
  curBuffer->push_back(vertex.pos.x);
  curBuffer->push_back(vertex.pos.y);
  curBuffer->push_back(vertex.pos.z);

  {
    uint8_t normx = vertex.norm.x*127+128;
    uint8_t normy = vertex.norm.y*127+128;
    uint8_t normz = vertex.norm.z*127+128;
    uint32_t compactNorm = normx | normy << 8 | normz << 16;
    curBuffer->push_back(*(float*)&compactNorm);
  }

  {
    uint8_t normx = vertex.flatNorm.x*127+128;
    uint8_t normy = vertex.flatNorm.y*127+128;
    uint8_t normz = vertex.flatNorm.z*127+128;
    uint32_t compactNorm = normx | normy << 8 | normz << 16;
    curBuffer->push_back(*(float*)&compactNorm);
  }
  curBuffer->push_back(vertex.norm.z);

  uint32_t compactIds = vertex.texIds[0] | (vertex.texIds[1] << 8) | (vertex.texIds[2] << 16) | (vertex.vId << 24);

  //std::cout << (int)vertex.ids[0] << ":" << (int)vertex.ids[1] << ":" << (int)vertex.ids[2] << ":" << coma<< "\n";
  curBuffer->push_back(*(float*)&compactIds);
  //Add the normal and texture ids
  uint8_t compactFace;
  switch(vertex.face)
  {
    case (FRONTF):   compactFace = 0b1001; break;
    case (BACKF):    compactFace = 0b0001; break;
    case (TOPF):     compactFace = 0b0010; break;
    case (BOTTOMF):  compactFace = 0b1010; break;
    case (RIGHTF):   compactFace = 0b0100; break;
    case (LEFTF):    compactFace = 0b1100; break;
  }
  uint8_t normandtex = compactFace | (vertex.ao << 4)| vertex.tb | vertex.rl;
  uint8_t texId = 0;


  uint32_t package = pack4chars(normandtex,texId,1,1);
  curBuffer->push_back(1);


  return numbVert;
}


bool BSP::empty()
{
  for(int i=0;i<CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;i++)
  {
    if(worldArray[i] != 0) return false;
  }
  return true;
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

    if(*VAO == 0 )
    {
      glGenVertexArrays(1, VAO);
      glGenBuffers(1, EBO);
      glGenBuffers(1, VBO);
    }
    /*
    glDeleteBuffers(1, VBO);
    glDeleteBuffers(1, EBO);
    glDeleteVertexArrays(1, VAO);

    glGenVertexArrays(1, VAO);
    glGenBuffers(1, EBO);
    glGenBuffers(1, VBO);
    */
    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER,*VBO);
    glBufferData(GL_ARRAY_BUFFER, curVert->size()*sizeof(float),&curVert->front(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, curInd->size()*sizeof(uint),&curInd->front(), GL_DYNAMIC_DRAW);

    int vertexSize = 8*sizeof(float);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,vertexSize, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE,vertexSize, (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,vertexSize, (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,vertexSize, (void*)(7*sizeof(float)));
    glEnableVertexAttribArray(3);



    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    geometryChanged = true;

  }
  curVert->clear();
  curInd->clear();
}

void BSP::render()
{
  setupBufferObjects(OPAQUE);
  setupBufferObjects(TRANSLUCENT);
}

void BSPNode::drawChunkOutline(const glm::vec3 &pos)
{
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    WireframeCube cube(glm::vec3(CHUNKSIZE*getPosition()+glm::ivec3(CHUNKSIZE/2)));
    cube.setScale(16);
    cube.setColor(glm::vec4(1));
    cube.draw(pos);
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void BSP::drawOpaque(const glm::vec3 &pos)
{



  if(oIndicesSize != 0)
  {
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(CHUNKSIZE*parent->getPosition())-pos);
    //std::cout << glm::to_string(glm::vec3(parent->chunkPos) - pos) << "\n";
    oShader.use();
    oShader.setMat4("model",model);
    glBindVertexArray(oVAO);
    glDrawElements(GL_TRIANGLES, oIndicesSize, GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

  }
}

void BSP::drawTranslucent(const glm::vec3 &pos)
{
  if(tIndicesSize)
  {
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(CHUNKSIZE*parent->getPosition())-pos);
    tShader.use();
    tShader.setMat4("model",model);
    glBindVertexArray(tVAO);
    glDrawElements(GL_TRIANGLES, tIndicesSize, GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
  }

}

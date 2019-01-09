
#include <string>
#include <fstream>
#include <sstream>

#define GLEW_STATIC
#include <GL/glew.h>

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

BSPNode::BSPNode(const glm::ivec3 &pos,const char *val) : curBSP(val,pos,this)
{

  //std::cout << "generating chunk" << x << ":" << y << ":" << z << " with size" << val.size() << "\n";
  //std::cout << "curNumber of chunks: " << totalChunks << "\n";
  chunkPos = pos;
  totalChunks++;
  toRender = false;
  toBuild = false;
  toDelete = false;

  leftChunk = NULL;
  rightChunk = NULL;
  frontChunk = NULL;
  backChunk = NULL;
  topChunk = NULL;
  bottomChunk = NULL;
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

#define DEFAULTBLOCKMODE TRANSLUCENT;
//If looking for a block outside of a chunks local coordinates use this;
RenderType BSPNode::blockVisibleTypeOOB(const glm::ivec3 &pos)
{
  //std::lock_guard<std::mutex> lock(BSPMutex);
  auto check = [&](std::shared_ptr<BSPNode> chunk,const glm::ivec3 &norm)
  {
    return chunk != NULL ? chunk->blockVisibleTypeOOB(pos+CHUNKSIZE*norm) : DEFAULTBLOCKMODE;
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
  else return blockVisibleType(pos);
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

void BSPNode::addBlock(const glm::ivec3 &pos, char id)
{
  std::lock_guard<std::recursive_mutex> lock(BSPMutex);
  curBSP.addBlock(pos,id);
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

//Get real world position
glm::ivec3 BSPNode::getRealWorldPosition()
{
  return (CHUNKSIZE*chunkPos) + glm::ivec3(CHUNKSIZE/2);
}

BSP::BSP(const char* data,const glm::ivec3 &pos,BSPNode* Parent)
{
  //blockOrigin = chunkLocalPos + CHUNKSIZE*(parent->chunkPos);
  modelMat = glm::translate(glm::mat4(1.0f),glm::vec3(CHUNKSIZE*pos));
  oIndicesSize = 0;
  tIndicesSize = 0;
  parent = Parent;

  using namespace std;

  oVBO = 0;
  oEBO = 0;
  oVAO = 0;
  tVBO = 0;
  tEBO = 0;
  tVAO = 0;

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
      curLength += ((uchar) data[index] << (j*8));
      index++;
    }
    for(int j = 0; j<curLength; j++)
    {
      if(i+j>numbOfBlocks)
      {
        std::cout << "ERROR CORRUPTED CHUNK AT " << glm::to_string(parent->chunkPos) <<"\n";
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


inline int pack4chars(char a, char b, char c, char d)
{
  return ((a << 24) | (b << 16) | (c << 8) | d);
}


AmbientOcclusion BSP::getAO(const glm::ivec3 &pos, Faces face, TextureSides top, TextureSides right)
{
  using namespace glm;
  ivec3 side1, side2;
  if(top == TOP)
  {
    switch(face)
    {
      case(TOPF):    side1 = ivec3(0,1,1); break;
      case(BOTTOMF): side1 = ivec3(0,-1,1); break;
      case(FRONTF):  side1 = ivec3(0,1,-1); break;
      case(BACKF):   side1 = ivec3(0,1,1); break;
      case(LEFTF):   side1 = ivec3(-1,1,0); break;
      case(RIGHTF):  side1 = ivec3(1,1,0); break;
    }
  }
  else
  {
    switch(face)
    {
      case(TOPF):    side1 = ivec3(0,1,-1); break;
      case(BOTTOMF): side1 = ivec3(0,-1,-1); break;
      case(FRONTF):  side1 = ivec3(0,-1,-1); break;
      case(BACKF):   side1 = ivec3(0,-1,1); break;
      case(LEFTF):   side1 = ivec3(-1,-1,0); break;
      case(RIGHTF):  side1 = ivec3(1,-1,0); break;
    }
  }

  if(right == RIGHT)
  {
    switch(face)
    {
      case(TOPF):    side2 = ivec3(1,1,0); break;
      case(BOTTOMF): side2 = ivec3(1,-1,0); break;
      case(FRONTF):  side2 = ivec3(1,0,-1); break;
      case(BACKF):   side2 = ivec3(1,0,1); break;
      case(LEFTF):   side2 = ivec3(-1,0,1); break;
      case(RIGHTF):  side2 = ivec3(1,0,1); break;
    }
  }
  else
  {
    switch(face)
    {
      case(TOPF):    side2 = ivec3(-1,1,0); break;
      case(BOTTOMF): side2 = ivec3(-1,-1,0); break;
      case(FRONTF):  side2 = ivec3(-1,0,-1); break;
      case(BACKF):   side2 = ivec3(-1,0,1); break;
      case(LEFTF):   side2 = ivec3(-1,0,-1); break;
      case(RIGHTF):  side2 = ivec3(1,0,-1); break;
    }
  }

  glm::ivec3 norm;
  switch(face)
  {
    case (FRONTF):  norm = ivec3(0,0,-1); break;
    case (BACKF):   norm = ivec3(0,0,1); break;
    case (TOPF):    norm = ivec3(0,1,0); break;
    case (BOTTOMF): norm = ivec3(0,-1,0); break;
    case (RIGHTF):  norm = ivec3(1,0,0); break;
    case (LEFTF):   norm = ivec3(-1,0,0); break;
  }

  ivec3 side1Pos = pos + side1;
  ivec3 side2Pos = pos + side2;

  bool side1Opacity = (OPAQUE==parent->blockVisibleTypeOOB(side1Pos));
  bool side2Opacity = (OPAQUE==parent->blockVisibleTypeOOB(side2Pos));

  if(side1Opacity && side2Opacity)
  {
    return FULLCOVER;
  }
  ivec3 corner = side1+side2-norm;
  ivec3 cornerPos = pos + corner;
  bool cornerOpacity = (OPAQUE==parent->blockVisibleTypeOOB(cornerPos));
  return static_cast<AmbientOcclusion> (side1Opacity+side2Opacity+cornerOpacity);
}

int BSP::addVertex(RenderType renderType,const glm::vec3 &pos,Faces face, TextureSides top, TextureSides right, char* AOvalue)
{
  std::vector<float>* curBuffer;
  if(renderType == OPAQUE)
  {
    curBuffer = &oVertices;
  }
  else if(renderType == TRANSLUCENT)
  {
    curBuffer = &tVertices;
  }
  else
  {
    std::cout << "Error\n";
    return 0;
  }
  int numbVert = curBuffer->size()/4;
  //Adds position vector
  curBuffer->push_back(pos.x);
  curBuffer->push_back(pos.y);
  curBuffer->push_back(pos.z);

  AmbientOcclusion ao = getAO(curLocalPos,face,top,right);
  *AOvalue = ao;
  //Add the normal and texture ids
  char compactFace;
  switch(face)
  {
    case (FRONTF):   compactFace = 0; break;
    case (BACKF):    compactFace = 1; break;
    case (TOPF):     compactFace = 2; break;
    case (BOTTOMF):  compactFace = 3; break;
    case (RIGHTF):   compactFace = 4; break;
    case (LEFTF):    compactFace = 5; break;
  }
  char normandtex = compactFace | (ao << 3)| top | right;
  char texId = curBlockid;
  char xtextcount = xdist;
  char ytextcount = ydist;

  int package = pack4chars(normandtex,texId,xtextcount,ytextcount);
  curBuffer->push_back(*(float*)&package);

  return numbVert;
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
  Block curBlock = ItemDatabase::blockDictionary[getBlock(pos)];
  if(curBlock.isLightSource)
  {
    //LightSource light;
    //light.color = curBlock.lightColor;
    //light.lightSize = curBlock.lightSize;
    //lightSources.add(pos,light);
  }
}

inline void BSP::delBlock(const glm::ivec3 &pos)
{
  worldArray.set(pos,0);
}

inline uchar BSP::getBlock(const glm::ivec3 &pos)
{
  return worldArray.get(pos);
}

void BSP::build()
{
  oVertices = std::vector<float>();
  oIndices = std::vector<uint>();
  tVertices = std::vector<float>();
  tIndices = std::vector<uint>();

  oVertices.reserve(20000);
  oIndices.reserve(10000);
  Array3D<BlockFace,32> arrayFaces;
  //Populate arrayFaces in order to determine all visible faces of the mesh
  for(int x = 0; x<CHUNKSIZE;x++)
  {
    for(int z = 0;z<CHUNKSIZE;z++)
    {
      for(int y = 0;y<CHUNKSIZE;y++)
      {
        glm::ivec3 chunkLocalPos = glm::ivec3(x,y,z);
         if(!blockExists(chunkLocalPos)) continue;
         RenderType renderType = blockVisibleType(chunkLocalPos);

         bool topNeigh = false;
         bool bottomNeigh = false;
         bool leftNeigh = false;
         bool rightNeigh = false;
         bool frontNeigh = false;
         bool backNeigh = false;

         bool defaultNull = true;

         auto check = [&](Faces face,const glm::ivec3& pos,bool* flag)
         {
           auto neigh = parent->getNeighbour(face);
           if(neigh != NULL)
           {
             if(renderType == neigh->blockVisibleType(pos)) *flag = true;
           }
           else if(defaultNull) *flag = true;
         };
         if(x+1 >= CHUNKSIZE)
         {
           check(RIGHTF,glm::ivec3(0,y,z),&rightNeigh);
         }
         else if(renderType == blockVisibleType(glm::ivec3(x+1,y,z))) rightNeigh = true;

         if(x-1 < 0)
         {
           check(LEFTF,glm::ivec3(CHUNKSIZE-1,y,z),&leftNeigh);
         }
         else if(renderType == blockVisibleType(glm::ivec3(x-1,y,z))) leftNeigh = true;

         if(y+1 >= CHUNKSIZE)
         {
           check(TOPF,glm::ivec3(x,0,z),&topNeigh);
         }
         else if(renderType == blockVisibleType(glm::ivec3(x,y+1,z))) topNeigh = true;

         if(y-1 < 0)
         {
           check(BOTTOMF,glm::ivec3(x,CHUNKSIZE-1,z),&bottomNeigh);
         }
         else if(renderType == blockVisibleType(glm::ivec3(x,y-1,z))) bottomNeigh = true;

         if(z+1 >= CHUNKSIZE)
         {
           check(BACKF,glm::ivec3(x,y,0),&backNeigh);
         }
         else if(renderType == blockVisibleType(glm::ivec3(x,y,z+1))) backNeigh = true;

         if(z-1 < 0)
         {
           check(FRONTF,glm::ivec3(x,y,CHUNKSIZE-1),&frontNeigh);
         }
         else if(renderType == blockVisibleType(glm::ivec3(x,y,z-1))) frontNeigh = true;

         BlockFace& curFace = arrayFaces.get(chunkLocalPos);
         if(!frontNeigh)  curFace.setFace(FRONTF);
         if(!backNeigh)   curFace.setFace(BACKF);
         if(!topNeigh)    curFace.setFace(TOPF);
         if(!bottomNeigh) curFace.setFace(BOTTOMF);
         if(!leftNeigh)   curFace.setFace(LEFTF);
         if(!rightNeigh)  curFace.setFace(RIGHTF);

       }
     }
   }

   glm::vec3 blockOrigin;
   char ao00,ao01,ao11,ao10;
   BlockFace curFace;
   char blockId;
   Block tempBlock;
   curLocalPos;
   glm::vec3 topleft, bottomleft,topright,bottomright;
   glm::vec3 tempVec;
   Faces normVec;
   char top;
   char right;
   RenderType renderType;
   glm::ivec3 rightVector;
   glm::ivec3 topVector;
   using namespace glm;
   int x,y,z;
   auto calcFace = [&](Faces face)
   {
     glm::ivec3 origin = glm::ivec3(x,y,z);
     AmbientOcclusion bottomleft = getAO(origin,face,BOTTOM,LEFT);
     AmbientOcclusion bottomright = getAO(origin,face,BOTTOM,RIGHT);
     AmbientOcclusion topleft = getAO(origin,face,TOP,LEFT);
     AmbientOcclusion topright = getAO(origin,face,TOP,RIGHT);
     auto checkMatching = [&](glm::ivec3 pos)
     {
       return (getBlock(pos) == blockId)
       && arrayFaces.get(pos).getFace(face)
       && topleft == getAO(pos,face,TOP,LEFT)
       && topright == getAO(pos,face,TOP,RIGHT)
       && bottomright == getAO(pos,face,BOTTOM,RIGHT)
       && bottomleft == getAO(pos,face,BOTTOM,LEFT);
     };


     arrayFaces.get(origin).delFace(face);
     glm::ivec3 r = rightVector;
     glm::ivec3 rpos = r + origin;
     while(glm::length(glm::vec3(origin*rightVector + r)) < CHUNKSIZE && checkMatching(rpos))
     {
       arrayFaces.get(rpos).delFace(face);
       r += rightVector;
       rpos += rightVector;
     }

     glm::ivec3 t =  topVector;
     glm::ivec3 tpos = glm::ivec3(x,y,z) + t;

     while(glm::length(glm::vec3(origin*topVector+t)) < CHUNKSIZE && checkMatching(tpos))
     {
       int rlength = glm::length(glm::vec3(r));
       bool clearRow = true;
       for(int i=0;i<rlength;i++)
       {
         glm::ivec3 rtpos = origin + t + rightVector*i;
         if(!checkMatching(rtpos)) clearRow = false;
       }

       if(clearRow)
       {
         for(int i=0;i<rlength;i++)
         {
           glm::ivec3 rtpos = origin + t + rightVector*i;
           arrayFaces.get(rtpos).delFace(face);
         }
         t += topVector;
         tpos += topVector;
       }
       else break;
     }
     top = glm::length(glm::vec3(t));
     right = glm::length(glm::vec3(r));
     xdist = top;
     ydist = right;
     curBlockid = tempBlock.getFace(face);
     normVec = face;
   };
   vec3 depthoffset = glm::vec3(0,1,0);
   const float offset = 0.005f;
   auto createVertices = [&](bool front)
   {
     bottomright = blockOrigin + depthoffset + (right+offset)*vec3(rightVector) + offset*vec3(-rightVector);
     topright = blockOrigin + depthoffset + (top+offset)*vec3(topVector) + (right+offset)*vec3(rightVector);
     bottomleft = blockOrigin + depthoffset + offset*vec3(-rightVector) + offset*vec3(-topVector);
     topleft = blockOrigin + depthoffset + (top+offset)*vec3(topVector) + offset*vec3(-rightVector);

     int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT,&ao10);
     int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT,&ao11);
     int index2 = addVertex(renderType,topleft,normVec,TOP,LEFT,&ao01);
     int index1 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT,&ao00);
     if(front)
     {
       if(ao00 + ao11 > ao01 + ao10)
       {
         addIndices(renderType,index2,index3,index4,index1);
       }
       else addIndices(renderType,index1,index2,index3,index4);
     }
     else
     {
       if(ao00 + ao11 > ao01 + ao10)
       {
         addIndices(renderType,index4,index3,index2,index1);
       }
       else addIndices(renderType,index3,index2,index1,index4);
     }

   };

  //Go through the array and test all faces and attempt to join them using greedy meshing
   for(x = 0; x<CHUNKSIZE;x++)
   {
     for(z = 0;z<CHUNKSIZE;z++)
     {
       for(y = 0;y<CHUNKSIZE;y++)
       {
         glm::ivec3 chunkLocalPos = glm::ivec3(x,y,z);
         renderType = blockVisibleType(chunkLocalPos);
         blockOrigin = chunkLocalPos + CHUNKSIZE*(parent->chunkPos);
         blockOrigin = chunkLocalPos;
         curFace  = arrayFaces.get(chunkLocalPos);
         //if(curFace.isEmpty()) continue;
         blockId = getBlock(chunkLocalPos);
         tempBlock = ItemDatabase::blockDictionary[blockId];
         curLocalPos = chunkLocalPos;

         if(curFace.getFace(TOPF))
         {
           rightVector = vec3(1,0,0);
           topVector = vec3(0,0,1);
           depthoffset = vec3(0,1,0);
           calcFace(TOPF);
           createVertices(true);
         }

         if(curFace.getFace(BOTTOMF))
         {
           rightVector = vec3(1,0,0);
           topVector = vec3(0,0,1);
           depthoffset = vec3(0,0,0);
           calcFace(BOTTOMF);
           createVertices(false);
         }

         if(curFace.getFace(RIGHTF))
         {
           rightVector = vec3(0,0,1);
           topVector = vec3(0,1,0);
           depthoffset = vec3(1,0,0);
           calcFace(RIGHTF);
           createVertices(true);
         }

         if(curFace.getFace(LEFTF))
         {
           rightVector = vec3(0,0,1);
           topVector = vec3(0,1,0);
           depthoffset = vec3(0,0,0);
           calcFace(LEFTF);
           createVertices(false);
         }

         if(curFace.getFace(BACKF))
         {
           rightVector = vec3(1,0,0);
           topVector = vec3(0,1,0);
           depthoffset = vec3(0,0,1);
           calcFace(BACKF);
           createVertices(false);
         }

         if(curFace.getFace(FRONTF))
         {
           rightVector = vec3(1,0,0);
           topVector = vec3(0,1,0);
           depthoffset = vec3(0,0,0);
           calcFace(FRONTF);
           createVertices(true);
         }
       }
     }
   }

}

void BSP::setupBufferObjects(RenderType type)
{
  int error = glGetError();
  if(error != 0)
  {
    std::cout << "OPENGL ERRORBEFORE BUFFESETUP" << error << ": in chunk pos"<< glm::to_string(parent->chunkPos) << "\n";
    std::cout << oVAO << ":" << oIndicesSize << "\n";
  }
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
  else
  {
    std::cout << "????\n";
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

    int vertexSize = 4*sizeof(float);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,vertexSize, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,vertexSize, (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

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
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(CHUNKSIZE*parent->chunkPos)-pos);
    //std::cout << glm::to_string(glm::vec3(parent->chunkPos) - pos) << "\n";
    shader->setMat4("model",model);
    glBindVertexArray(oVAO);
    glDrawElements(GL_TRIANGLES, oIndicesSize, GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

    int error = glGetError();
    if(error != 0)
    {
      std::cout << "OPENGL ERROR" << error << ": in chunk pos"<< glm::to_string(parent->chunkPos) << "\n";
      //std::cout << oVAO << ":" << oIndicesSize << "\n";
    }

  }
}

void BSP::drawTranslucent(Shader* shader,const glm::vec3 &pos)
{
  if(tIndicesSize)
  {
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(CHUNKSIZE*parent->chunkPos)-pos);
    shader->setMat4("model",model);
    glBindVertexArray(tVAO);
    glDrawElements(GL_TRIANGLES, tIndicesSize, GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
  }

}

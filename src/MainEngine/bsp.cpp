
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
  //totalChunks--;
}

BSPNode::BSPNode(const glm::ivec3 &pos,const char *val)
{
  //std::cout << "generating chunk" << x << ":" << y << ":" << z << " with size" << val.size() << "\n";
  //std::cout << "curNumber of chunks: " << totalChunks << "\n";
  //totalChunks++;
  BSPMutex.lock();
  curBSP = BSP(pos,val,this);
  toRender = false;
  toBuild = false;
  toDelete = false;
  BSPMutex.unlock();
}

void BSPNode::build()
{
  BSPMutex.lock();
  curBSP.build();
  toRender = true;
  toBuild = false;
  BSPMutex.unlock();
}

void BSPNode::drawOpaque()
{
  if(toRender == true)
  {
    if(BSPMutex.try_lock())
    {
      curBSP.swapBuffers();
      curBSP.render();
      toRender = false;
      BSPMutex.unlock();
    }
  }
  curBSP.drawOpaque();
}

void BSPNode::drawTranslucent()
{

  if(toRender == true)
  {
     curBSP.render();
     toRender = false;
  }
  curBSP.drawTranslucent();
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
  BSPMutex.unlock();

}

#define DEFAULTBLOCKMODE TRANSLUCENT;
//If looking for a block outside of a chunks local coordinates use this;
RenderType BSPNode::blockVisibleTypeOOB(const glm::ivec3 &pos)
{
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
  return curBSP.blockExists(pos);
}

void BSPNode::delBlock(const glm::ivec3 &pos)
{
  curBSP.delBlock(pos);
}

void BSPNode::addBlock(const glm::ivec3 &pos, char id)
{
  curBSP.addBlock(pos,id);
}

RenderType BSPNode::blockVisibleType(const glm::ivec3 &pos)
{
  return curBSP.blockVisibleType(pos);
}


BSP::BSP(const glm::ivec3 &pos,const char* data, BSPNode* Parent)
{
  parent = Parent;
  oVerticesBuffer = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  oIndicesBuffer  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
  tVerticesBuffer = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  tIndicesBuffer  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);

  oVertices = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  oIndices  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
  tVertices = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  tIndices = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
  chunkPos = pos;
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
      curLength += ((uchar) data[index] << (j*8));
      index++;
    }
    for(int j = 0; j<curLength; j++)
    {
      if(i+j>numbOfBlocks)
      {
        std::cout << "ERROR CORRUPTED CHUNK AT " << glm::to_string(chunkPos) <<"\n";
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
  glDeleteBuffers(1,&oVBO);
  glDeleteBuffers(1,&oEBO);
  glDeleteVertexArrays(1,&oVAO);

  glDeleteBuffers(1,&tVBO);
  glDeleteBuffers(1,&tEBO);
  glDeleteVertexArrays(1,&tVAO);
}

void BSP::swapBuffers()
{
  oVertices = oVerticesBuffer;
  oIndices = oIndicesBuffer;
  tVertices = tVerticesBuffer;
  tIndices = tIndicesBuffer;

  oVerticesBuffer = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  oIndicesBuffer  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
  tVerticesBuffer = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  tIndicesBuffer  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
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
      case(TOPF): side1 = ivec3(0,1,1); break;
      case(BOTTOMF): side1 = ivec3(0,-1,1); break;
      case(FRONTF): side1 = ivec3(0,1,-1); break;
      case(BACKF): side1 = ivec3(0,1,1); break;
      case(LEFTF): side1 = ivec3(-1,1,0); break;
      case(RIGHTF): side1 = ivec3(1,1,0); break;
    }
  }
  else
  {
    switch(face)
    {
      case(TOPF): side1 = ivec3(0,1,-1); break;
      case(BOTTOMF): side1 = ivec3(0,-1,-1); break;
      case(FRONTF): side1 = ivec3(0,-1,-1); break;
      case(BACKF): side1 = ivec3(0,-1,1); break;
      case(LEFTF): side1 = ivec3(-1,-1,0); break;
      case(RIGHTF): side1 = ivec3(1,-1,0); break;
    }
  }

  if(right == RIGHT)
  {
    switch(face)
    {
      case(TOPF): side2 = ivec3(1,1,0); break;
      case(BOTTOMF): side2 = ivec3(1,-1,0); break;
      case(FRONTF): side2 = ivec3(1,0,-1); break;
      case(BACKF): side2 = ivec3(1,0,1); break;
      case(LEFTF): side2 = ivec3(-1,0,1); break;
      case(RIGHTF): side2 = ivec3(1,0,1); break;
    }
  }
  else
  {
    switch(face)
    {
      case(TOPF): side2 = ivec3(-1,1,0); break;
      case(BOTTOMF): side2 = ivec3(-1,-1,0); break;
      case(FRONTF): side2 = ivec3(-1,0,-1); break;
      case(BACKF): side2 = ivec3(-1,0,1); break;
      case(LEFTF): side2 = ivec3(-1,0,-1); break;
      case(RIGHTF): side2 = ivec3(1,0,-1); break;
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

inline int BSP::addVertex(RenderType renderType,const glm::vec3 &pos,Faces face, TextureSides top, TextureSides right, char* AOvalue)
{
  std::shared_ptr<std::vector<float>> curBuffer;
  if(renderType == OPAQUE)
  {
    curBuffer = oVerticesBuffer;
  }
  else if(renderType == TRANSLUCENT)
  {
    curBuffer = tVerticesBuffer;
  }

  int numbVert = oVerticesBuffer->size()/4;
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

inline void BSP::addIndices(RenderType renderType,int index1, int index2, int index3, int index4)
{
  std::shared_ptr<std::vector<uint>> curBuffer;
  if(renderType == OPAQUE)
  {
    curBuffer = oIndicesBuffer;
  }
  else if(renderType == TRANSLUCENT)
  {
    curBuffer = tIndicesBuffer;
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
  return worldArray.get(pos) == 0 ? false : true;
}

RenderType BSP::blockVisibleType(const glm::ivec3 &pos)
{
  return ItemDatabase::blockDictionary[getBlock(pos)].visibleType;
}

void BSP::addBlock(const glm::ivec3 &pos, char id)
{
  worldArray.set(pos,id);
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
  oVerticesBuffer->clear();
  oIndicesBuffer->clear();
  tVerticesBuffer->clear();
  tIndicesBuffer->clear();
  Array3D<BlockFace,32> arrayFaces;
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


         if(x+1 >= CHUNKSIZE)
         {
           if(parent->rightChunk != NULL)
           {
             if(renderType == parent->rightChunk->blockVisibleType(glm::ivec3(0,y,z))) rightNeigh = true;
           }
           else if(defaultNull) rightNeigh = true;
         }
         else if(renderType == blockVisibleType(glm::ivec3(x+1,y,z))) rightNeigh = true;

         if(x-1 < 0)
         {
           if(parent->leftChunk != NULL)
           {
             if(renderType == parent->leftChunk->blockVisibleType(glm::ivec3(CHUNKSIZE-1,y,z))) leftNeigh = true;
           }
           else if(defaultNull) leftNeigh = true;
         }
         else if(renderType == blockVisibleType(glm::ivec3(x-1,y,z))) leftNeigh = true;

         if(y+1 >= CHUNKSIZE)
         {
           if(parent->topChunk != NULL)
           {
              if(renderType == parent->topChunk->blockVisibleType(glm::ivec3(x,0,z))) topNeigh = true;
           }
           else if(defaultNull) topNeigh = true;
         }
         else if(renderType == blockVisibleType(glm::ivec3(x,y+1,z))) topNeigh = true;

         if(y-1 < 0)
         {
          if(parent->bottomChunk != NULL)
          {
            if(renderType == parent->bottomChunk->blockVisibleType(glm::ivec3(x,CHUNKSIZE-1,z))) bottomNeigh = true;
          }
          else if(defaultNull) bottomNeigh = true;
         }
         else if(renderType == blockVisibleType(glm::ivec3(x,y-1,z))) bottomNeigh = true;

         if(z+1 >= CHUNKSIZE)
         {
           if(parent->backChunk != NULL)
           {
             if(renderType == parent->backChunk->blockVisibleType(glm::ivec3(x,y,0)))backNeigh = true;
           }
           else if(defaultNull) backNeigh = true;
         }
         else if(renderType == blockVisibleType(glm::ivec3(x,y,z+1))) backNeigh = true;

         if(z-1 < 0)
         {
           if(parent->frontChunk != NULL)
           {
             if(renderType == parent->frontChunk->blockVisibleType(glm::ivec3(x,y,CHUNKSIZE-1))) frontNeigh = true;
           }
           else if(defaultNull) frontNeigh = true;
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

   for(x = 0; x<CHUNKSIZE;x++)
   {
     for(z = 0;z<CHUNKSIZE;z++)
     {
       for(y = 0;y<CHUNKSIZE;y++)
       {
         glm::ivec3 chunkLocalPos = glm::ivec3(x,y,z);
         renderType = blockVisibleType(chunkLocalPos);
         blockOrigin = chunkLocalPos + CHUNKSIZE*chunkPos;
         curFace  = arrayFaces.get(chunkLocalPos);
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
  std::shared_ptr<std::vector<float>> curVert;
  std::shared_ptr<std::vector<uint>> curInd;
  if(type == OPAQUE)
  {
    curVert = oVertices;
    curInd = oIndices;
  }
  else if(type == TRANSLUCENT)
  {
    curVert = tVertices;
    curInd = tIndices;
  }
  if(curInd->size() != 0)
  {
    glDeleteBuffers(1, &oVBO);
    glDeleteBuffers(1, &oEBO);
    glDeleteVertexArrays(1, &oVAO);

    glGenVertexArrays(1, &oVAO);
    glGenBuffers(1, &oEBO);
    glGenBuffers(1, &oVBO);
    glBindVertexArray(oVAO);

    glBindBuffer(GL_ARRAY_BUFFER,oVBO);
    glBufferData(GL_ARRAY_BUFFER, curVert->size()*sizeof(float),&curVert->front(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,oEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, curInd->size()*sizeof(uint),&curInd->front(), GL_DYNAMIC_DRAW);

    int vertexSize = 4*sizeof(float);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,vertexSize, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,vertexSize, (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    geometryChanged = true;
  }
}

inline void BSP::render()
{
  setupBufferObjects(OPAQUE);

}

void BSP::drawOpaque()
{

  if(oIndices->size() != 0)
  {

    glBindVertexArray(oVAO);
    glDrawElements(GL_TRIANGLES, oIndices->size(), GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
    //If it errors, re render it
    //TODO: find the source of the bug so the chunk building never fails
    //Cause this is just a bandaid solution lmao
    int error = glGetError();
    if(error != 0)
    {
      std::cout << "OPENGL ERROR" << error << ": in chunk pos"<< glm::to_string(chunkPos) << "\n";
      render();
    }

  }
}

void BSP::drawTranslucent()
{
  glBindVertexArray(tVAO);
  glDrawElements(GL_TRIANGLES, tIndices->size(), GL_UNSIGNED_INT,0);
  glBindVertexArray(0);
}

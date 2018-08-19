
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

BSPNode::BSPNode(int x,int y, int z,const std::string &wName,const char *val)
{
  //std::cout << "generating chunk" << x << ":" << y << ":" << z << " with size" << val.size() << "\n";
  //std::cout << "curNumber of chunks: " << totalChunks << "\n";
  //totalChunks++;
  BSPMutex.lock();
  curBSP = BSP(x,y,z,wName,val,this);
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
RenderType BSPNode::blockVisibleTypeOOB(int x,int y, int z)
{
  if(x >= CHUNKSIZE)
  {
    return rightChunk != NULL ? rightChunk->blockVisibleTypeOOB(x-CHUNKSIZE,y,z) : DEFAULTBLOCKMODE;
  }
  else if(x < 0)
  {
    return leftChunk != NULL ? leftChunk->blockVisibleTypeOOB(x+CHUNKSIZE,y,z) : DEFAULTBLOCKMODE;
  }
  else if(y >= CHUNKSIZE)
  {
    return topChunk != NULL ? topChunk->blockVisibleTypeOOB(x,y-CHUNKSIZE,z) : DEFAULTBLOCKMODE;
  }
  else if(y < 0)
  {
    return bottomChunk != NULL ? bottomChunk->blockVisibleTypeOOB(x,y+CHUNKSIZE,z) : DEFAULTBLOCKMODE;
  }
  else if(z >= CHUNKSIZE)
  {
    return backChunk != NULL ? backChunk->blockVisibleTypeOOB(x,y,z-CHUNKSIZE) : DEFAULTBLOCKMODE;
  }
  else if(z < 0)
  {
    return frontChunk != NULL ? frontChunk->blockVisibleTypeOOB(x,y,z+CHUNKSIZE) : DEFAULTBLOCKMODE;
  }
  else return blockVisibleType(x,y,z);
}

bool BSPNode::blockExists(int x, int y, int z)
{
  return curBSP.blockExists(x, y, z);
}

void BSPNode::delBlock(int x, int y, int z)
{
  curBSP.delBlock(x, y, z);
}

void BSPNode::addBlock(int x, int y, int z, int id)
{
  curBSP.addBlock(x,y,z,id);
}

RenderType BSPNode::blockVisibleType(int x, int y, int z)
{
  return curBSP.blockVisibleType(x, y, z);
}


BSP::BSP(int x, int y, int z,const std::string &wName,const char* data, BSPNode* Parent)
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
  xCoord = x;
  yCoord = y;
  zCoord = z;
  using namespace std;
  worldName = wName;

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
        std::cout << "ERROR CORRUPTED CHUNK AT " << x << ":" << y << ":" << z <<"\n";
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

  oVerticesBuffer = std::shared_ptr<std::vector<GLfloat>> (new std::vector<GLfloat>);
  oIndicesBuffer  = std::shared_ptr<std::vector<GLuint>> (new std::vector<GLuint>);
  tVerticesBuffer = std::shared_ptr<std::vector<GLfloat>> (new std::vector<GLfloat>);
  tIndicesBuffer  = std::shared_ptr<std::vector<GLuint>> (new std::vector<GLuint>);
}

inline int pack4chars(char a, char b, char c, char d)
{
  return ((a << 24) | (b << 16) | (c << 8) | d);
}


AmbientOcclusion BSP::getAO(glm::ivec3 pos, Faces face, TextureSides top, TextureSides right)
{
  using namespace glm;
  ivec3 side1, side2;
  if(top == TOP)
  {
    switch(face)
    {
      case(TOPF): side1 = ivec3(0,1,1); break;
      case(BOTTOMF): side1 = ivec3(0,-1,-1); break;
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
      case(BOTTOMF): side1 = ivec3(0,-1,1); break;

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
      case(BACKF): side2 = ivec3(-1,0,1); break;
      case(LEFTF): side2 = ivec3(-1,0,-1); break;
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
      case(BACKF): side2 = ivec3(1,0,1); break;
      case(LEFTF): side2 = ivec3(-1,0,1); break;
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
  ivec3 corner = side1+side2-norm;
  //std::cout << to_string(side1) << to_string(side2) << to_string(norm) << to_string(corner)<< "\n";

  ivec3 cornerPos = pos + corner;
  ivec3 side1Pos = pos + side1;
  ivec3 side2Pos = pos + side2;
  //std::cout << to_string(pos) <<to_string(side1Pos) << to_string(side2Pos) << to_string(cornerPos) << "\n";
  bool cornerOpacity = (OPAQUE==parent->blockVisibleTypeOOB(cornerPos.x,cornerPos.y,cornerPos.z));
  bool side1Opacity = (OPAQUE==parent->blockVisibleTypeOOB(side1Pos.x,side1Pos.y,side1Pos.z));
  bool side2Opacity = (OPAQUE==parent->blockVisibleTypeOOB(side2Pos.x,side2Pos.y,side2Pos.z));

  if(side1Opacity && side2Opacity)
  {
    return FULLCOVER;
  }
  //std::cout << (side1Opacity+side2Opacity+cornerOpacity) << "\n";
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
  //std::cout << ao << "\n";
  //ao = NOAO;
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

  //std::cout << "Norm is" << face << "\n";
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


inline bool BSP::blockExists(int x, int y, int z)
{
  return worldArray.get(x,y,z) == 0 ? false : true;
}

RenderType BSP::blockVisibleType(int x, int y, int z)
{
  return ItemDatabase::blockDictionary[getBlock(x,y,z)].visibleType;
}

void BSP::addBlock(int x, int y, int z, char id)
{
  worldArray.set(x,y,z,id);
}

inline void BSP::delBlock(int x, int y, int z)
{
  std::cout << "destroying block" << x << ":" << y << ":" << z << "\n";
  worldArray.set(x,y,z,0);
}

inline uchar BSP::getBlock(int x, int y, int z)
{
  return worldArray.get(x,y,z);
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

         if(!blockExists(x,y,z)) continue;
         RenderType renderType = blockVisibleType(x,y,z);

         float realX = x+CHUNKSIZE*xCoord;
         float realY = y+CHUNKSIZE*yCoord;
         float realZ = z+CHUNKSIZE*zCoord;

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
             if(renderType == parent->rightChunk->blockVisibleType(0,y,z)) rightNeigh = true;
           }
           else if(defaultNull) rightNeigh = true;
         }
         else if(renderType == blockVisibleType(x+1,y,z)) rightNeigh = true;

         if(x-1 < 0)
         {
           if(parent->leftChunk != NULL)
           {
             if(renderType == parent->leftChunk->blockVisibleType(CHUNKSIZE-1,y,z)) leftNeigh = true;
           }
           else if(defaultNull) leftNeigh = true;
         }
         else if(renderType == blockVisibleType(x-1,y,z)) leftNeigh = true;

         if(y+1 >= CHUNKSIZE)
         {
           if(parent->topChunk != NULL)
           {
              if(renderType == parent->topChunk->blockVisibleType(x,0,z)) topNeigh = true;
           }
           else if(defaultNull) topNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y+1,z)) topNeigh = true;

         if(y-1 < 0)
         {
          if(parent->bottomChunk != NULL)
          {
            if(renderType == parent->bottomChunk->blockVisibleType(x,CHUNKSIZE-1,z)) bottomNeigh = true;
          }
          else if(defaultNull) bottomNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y-1,z)) bottomNeigh = true;

         if(z+1 >= CHUNKSIZE)
         {
           if(parent->backChunk != NULL)
           {
             if(renderType == parent->backChunk->blockVisibleType(x,y,0))backNeigh = true;
           }
           else if(defaultNull) backNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y,z+1)) backNeigh = true;

         if(z-1 < 0)
         {
           if(parent->frontChunk != NULL)
           {
             if(renderType == parent->frontChunk->blockVisibleType(x,y,CHUNKSIZE-1)) frontNeigh = true;
           }
           else if(defaultNull) frontNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y,z-1)) frontNeigh = true;

         BlockFace& curFace = arrayFaces.get(x,y,z);
         if(!frontNeigh)  curFace.setFace(FRONTF);
         if(!backNeigh)   curFace.setFace(BACKF);
         if(!topNeigh)    curFace.setFace(TOPF);
         if(!bottomNeigh) curFace.setFace(BOTTOMF);
         if(!leftNeigh)   curFace.setFace(LEFTF);
         if(!rightNeigh)  curFace.setFace(RIGHTF);

       }
     }
   }



   for(int x = 0; x<CHUNKSIZE;x++)
   {
     for(int z = 0;z<CHUNKSIZE;z++)
     {
       for(int y = 0;y<CHUNKSIZE;y++)
       {
         const float offset = 0.003f;
         RenderType renderType = blockVisibleType(x,y,z);
         float realX = x+CHUNKSIZE*xCoord;
         float realY = y+CHUNKSIZE*yCoord;
         float realZ = z+CHUNKSIZE*zCoord;

         char ao00,ao01,ao11,ao10;
         BlockFace curFace = arrayFaces.get(x,y,z);
         char blockId = getBlock(x,y,z);
         Block tempBlock = ItemDatabase::blockDictionary[blockId];
         curLocalPos = glm::ivec3(x,y,z);
         glm::vec3 topleft, bottomleft,topright,bottomright;
         glm::vec3 tempVec;
         Faces normVec;

         xdist = 1;
         ydist = 1;
         if(curFace.getFace(TOPF))
         {
           char top = 1;
           char right = 1;
           calcFace(x,y,z,&arrayFaces,&top,&right,blockId,TOPF,glm::ivec3(0,0,1),glm::ivec3(1,0,0));

           bottomright  = glm::vec3(realX+right+offset,realY+1.0f,realZ-offset);
           topright = glm::vec3(realX+right+offset,realY+1.0f,realZ+top+offset);
           bottomleft     = glm::vec3(realX-offset,realY+1.0f,realZ-offset);
           topleft    = glm::vec3(realX-offset,realY+1.0f,realZ+top+offset);

           normVec = TOPF;
           xdist = top;
           ydist = right;
           curBlockid = tempBlock.getTop();

           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT,&ao10);
           int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT,&ao11);
           int index2 = addVertex(renderType,topleft,normVec,TOP,LEFT,&ao01);
           int index1 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT,&ao00);
           if(ao00 + ao11 > ao01 + ao10)
           {
             addIndices(renderType,index2,index3,index4,index1);

           }
           else addIndices(renderType,index1,index2,index3,index4);

         }

         if(curFace.getFace(BOTTOMF))
         {
           char top = 1;
           char right = 1;
           calcFace(x,y,z,&arrayFaces,&top,&right,blockId,BOTTOMF,glm::ivec3(0,0,1),glm::ivec3(1,0,0));

           topleft     = glm::vec3(realX-offset,realY,realZ-offset);
           topright  = glm::vec3(realX+right+offset,realY,realZ-offset);
           bottomleft    = glm::vec3(realX-offset,realY,realZ+top+offset);
           bottomright = glm::vec3(realX+right+offset,realY,realZ+top+offset);
           normVec = BOTTOMF;
           curBlockid = tempBlock.getBottom();
           xdist = top;
           ydist = right;

           int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT,&ao11);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT,&ao01);
           int index2 = addVertex(renderType,topleft,normVec,TOP,LEFT,&ao10);
           int index1 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT,&ao00);

           if(ao00 + ao11 > ao01 + ao10)
           {
             addIndices(renderType,index2,index3,index4,index1);

           }
           else addIndices(renderType,index1,index2,index3,index4);
         }

         if(curFace.getFace(RIGHTF))
         {
           char top = 1;
           char right = 1;
           calcFace(x,y,z,&arrayFaces,&top,&right,blockId,RIGHTF,glm::ivec3(0,1,0),glm::ivec3(0,0,1));

           bottomleft     = glm::vec3(realX+1.0f,realY-offset,realZ-offset);
           topleft  = glm::vec3(realX+1.0f,realY+top+offset,realZ-offset);
           bottomright    = glm::vec3(realX+1.0f,realY-offset,realZ+right+offset);
           topright = glm::vec3(realX+1.0f,realY+top+offset,realZ +right+offset);
           normVec = RIGHTF;
           xdist = top;
           ydist = right;
           curBlockid = tempBlock.getRight();
           int index1 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT,&ao00);
           int index2 = addVertex(renderType,topleft,normVec,TOP,LEFT,&ao01);
           int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT,&ao11);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT,&ao10);

           if(ao00 + ao11 > ao01 + ao10)
           {
             addIndices(renderType,index2,index3,index4,index1);

           }
           else addIndices(renderType,index1,index2,index3,index4);
         }

         if(curFace.getFace(LEFTF))
         {
           char top = 1;
           char right = 1;
           calcFace(x,y,z,&arrayFaces,&top,&right,blockId,LEFTF,glm::ivec3(0,1,0),glm::ivec3(0,0,1));

           bottomright     = glm::vec3(realX,realY-offset,realZ-offset);
           topright  = glm::vec3(realX,realY+top+offset,realZ-offset);
           bottomleft    = glm::vec3(realX,realY-offset,realZ+right+offset);
           topleft = glm::vec3(realX,realY+top+offset,realZ +right+offset);
           normVec = LEFTF;
           xdist   = top;
           ydist   = right;
           curBlockid = tempBlock.getLeft();
           int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT,&ao11);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT,&ao01);
           int index2 = addVertex(renderType,topleft,normVec,TOP,LEFT,&ao10);
           int index1 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT,&ao00);

           if(ao00 + ao11 > ao01 + ao10)
           {
             addIndices(renderType,index2,index3,index4,index1);

           }
           else addIndices(renderType,index1,index2,index3,index4);
         }



         if(curFace.getFace(BACKF))
         {
           char top = 1;
           char right = 1;
           calcFace(x,y,z,&arrayFaces,&top,&right,blockId,BACKF,glm::ivec3(0,1,0),glm::ivec3(1,0,0));

           bottomleft  = glm::vec3(realX+right+offset,realY-offset,realZ+1.0f);
           bottomright     = glm::vec3(realX-offset,realY-offset,realZ+1.0f);
           topright    = glm::vec3(realX-offset,realY+top+offset,realZ+1.0f);
           topleft = glm::vec3(realX+right+offset,realY+top+offset,realZ+1.0f);

           curBlockid = tempBlock.getBack();
           normVec = BACKF;
           xdist = top;
           ydist = right;
           int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT,&ao11);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT,&ao01);
           int index2 = addVertex(renderType,topleft,normVec,TOP,LEFT,&ao10);
           int index1 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT,&ao00);
           if(ao00 + ao11 > ao01 + ao10)
           {
             addIndices(renderType,index2,index3,index4,index1);

           }
           else addIndices(renderType,index1,index2,index3,index4);
         }


         if(curFace.getFace(FRONTF))
         {
           char top,right;
           top = 1,right =1;
           calcFace(x,y,z,&arrayFaces,&top,&right,blockId,FRONTF,glm::ivec3(0,1,0),glm::ivec3(1,0,0));
           xdist = top;
           ydist = right;
           bottomleft     = glm::vec3(realX-offset,realY-offset,realZ);
           topleft    = glm::vec3(realX-offset,realY+top+offset,realZ);
           topright = glm::vec3(realX+right+offset,realY+top+offset,realZ);
           bottomright  = glm::vec3(realX+right+offset,realY-offset,realZ);



           curBlockid = tempBlock.getFront();
           normVec = FRONTF;


           int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT,&ao11);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT,&ao01);
           int index2 = addVertex(renderType,topleft,normVec,TOP,LEFT,&ao10);
           int index1 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT,&ao00);

           if(ao00 + ao11 > ao01 + ao10)
           {
             addIndices(renderType,index2,index3,index4,index1);

           }
           else addIndices(renderType,index1,index2,index3,index4);

         }
       }
     }
   }
}



void BSP::calcFace(const int x, const int y, const int z, Array3D<BlockFace,32>* arrayFaces,
                   char* outtop, char* outright, char blockId, Faces face,
                   const glm::ivec3& topVector, const glm::ivec3& rightVector)
{
  using namespace glm;
  AmbientOcclusion bottomleft = getAO(ivec3(x,y,z),face,BOTTOM,LEFT);
  AmbientOcclusion bottomright = getAO(ivec3(x,y,z),face,BOTTOM,RIGHT);
  AmbientOcclusion topleft = getAO(ivec3(x,y,z),face,TOP,LEFT);
  AmbientOcclusion topright = getAO(ivec3(x,y,z),face,TOP,RIGHT);
  auto checkMatching = [&](glm::ivec3 pos)
  {
    return (getBlock(pos.x,pos.y,pos.z) == blockId)
    && arrayFaces->get(pos.x,pos.y,pos.z).getFace(face)
    && topleft == getAO(pos,face,TOP,LEFT)
    && topright == getAO(pos,face,TOP,RIGHT)
    && bottomright == getAO(pos,face,BOTTOM,RIGHT)
    && bottomleft == getAO(pos,face,BOTTOM,LEFT);

  };

  arrayFaces->get(x,y,z).delFace(face);
  glm::ivec3 origin = glm::ivec3(x,y,z);
  glm::ivec3 r = rightVector;
  glm::ivec3 rpos = r + origin;
  while(glm::length(glm::vec3(origin*rightVector + r)) < CHUNKSIZE && checkMatching(rpos))
  {
    arrayFaces->get(rpos.x,rpos.y,rpos.z).delFace(face);
    r += rightVector;
    rpos += rightVector;
  }

  glm::ivec3 t =  topVector;
  glm::ivec3 tpos = glm::ivec3(x,y,z) + t;

  while(glm::length(glm::vec3(origin*topVector+t)) < CHUNKSIZE && checkMatching(tpos))
  {

    bool clearRow = true;
    for(int i=0;i<glm::length(glm::vec3(r));i++)
    {
      glm::ivec3 rtpos = origin + t + rightVector*i;
      if(!checkMatching(rtpos)) clearRow = false;
    }

    if(clearRow)
    {
      for(int i=0;i<glm::length(glm::vec3(r));i++)
      {
        glm::ivec3 rtpos = origin + t + rightVector*i;
        arrayFaces->get(rtpos.x,rtpos.y,rtpos.z).delFace(face);
      }
      t += topVector;
      tpos += topVector;
    }
    else break;
  }
  *outtop = glm::length(glm::vec3(t));
  *outright = glm::length(glm::vec3(r));

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
      std::cout << "OPENGL ERROR" << error << ": in chunk pos"<< xCoord << ":" << yCoord << ":" << zCoord <<"\n";
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

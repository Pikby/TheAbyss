
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


BSPNode::BSPNode(int x,int y,int z,const std::string &wName)
{
  //std::cout << totalChunks << "\n";

  curBSP = BSP(x,y,z,wName);
  totalChunks++;
  isGenerated = false;
  inUse = false;
  toRender = false;
  toBuild = false;
  toDelete = false;
}

BSPNode::~BSPNode()
{
  totalChunks--;
}

BSPNode::BSPNode(int x,int y, int z,const std::string &wName,const std::string &val)
{
  //std::cout << "generating chunk" << x << ":" << y << ":" << z << " with size" << val.size() << "\n";
  //std::cout << "curNumber of chunks: " << totalChunks << "\n";
  totalChunks++;
  BSPMutex.lock();
  curBSP = BSP(x,y,z,wName,val);
  isGenerated = false;
  inUse = false;
  toRender = false;
  toBuild = false;
  toDelete = false;
  BSPMutex.unlock();
}

void BSPNode::generateTerrain()
{
  curBSP.generateTerrain();
}

void BSPNode::build()
{
  BSPMutex.lock();
  inUse = true;
  curBSP.build(rightChunk,leftChunk,topChunk,bottomChunk,frontChunk,backChunk);
  toRender = true;
  toBuild = false;
  inUse = false;
  BSPMutex.unlock();
}

void BSPNode::drawOpaque()
{
  if(toRender == true)
  {
     curBSP.render();
     toRender = false;
  }
  curBSP.drawOpaque();
}

void BSPNode::drawTranslucent()
{

  inUse = true;
  if(toRender == true)
  {
     curBSP.render();
     toRender = false;
  }
  curBSP.drawTranslucent();
  inUse = false;
}

void BSPNode::saveChunk()
{
  curBSP.saveChunk();
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

int BSPNode::blockVisibleType(int x, int y, int z)
{
  return curBSP.blockVisibleType(x, y, z);
}


BSP::BSP(int x, int y, int z,const std::string &wName,const std::string &val)
{



  xCoord = x;
  yCoord = y;
  zCoord = z;
  using namespace std;
  worldName = wName;

  const int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
  int i = 0;
  char curId = 0;
  unsigned short curLength = 0;
  const char* data = val.c_str();
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
      }
      worldArray[i+j] = curId;
    }
    i+= curLength;
  }
  oVerticesBuffer = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  oIndicesBuffer  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
  tVerticesBuffer = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  tIndicesBuffer  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);

  oVertices = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  oIndices  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
  tVertices = std::shared_ptr<std::vector<float>> (new std::vector<float>);
  tIndices = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
}

BSP::BSP(int x, int y, int z,const std::string &wName)
{
    xCoord = x;
    yCoord = y;
    zCoord = z;
    worldName = wName;
    //Intialize Buffers

    using namespace std;
    //The directoy to the chunk to be saved
    string directory = "saves/" + worldName + "/chunks/";
    string chunkName = to_string(x) + '_' + to_string(y) + '_' + to_string(z) + ".dat";
    string chunkPath = directory+chunkName;

    //Max size of a chunk
    int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
    ifstream ichunk(chunkPath,ios::binary);

    //Checks if the file exists
    if(!ichunk.is_open())
    {
      /*
      generateTerrain();
      ichunk.close();
      ofstream ochunk(chunkPath,ios::binary);

      string compressed = compressChunk();
      ochunk << compressed;
      ochunk.close();
      */
    }
    else
    {
      int i = 0;
      char curId=0;
      unsigned short curLength=0;
      while(i<numbOfBlocks)
      {

        ichunk.read((char*) &curId,sizeof(curId));
        ichunk.read((char*) &curLength,sizeof(curLength));

        for(int j = 0; j<curLength; j++)
        {
          worldArray[i+j] = curId;
        }
        i+= curLength;
      }
      ichunk.close();
    }
    oVerticesBuffer = std::shared_ptr<std::vector<float>> (new std::vector<float>);
    oIndicesBuffer  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
    tVerticesBuffer = std::shared_ptr<std::vector<float>> (new std::vector<float>);
    tIndicesBuffer  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);

    oVertices = std::shared_ptr<std::vector<float>> (new std::vector<float>);
    oIndices  = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
    tVertices = std::shared_ptr<std::vector<float>> (new std::vector<float>);
    tIndices = std::shared_ptr<std::vector<uint>> (new std::vector<uint>);
}

inline std::string BSP::compressChunk()
{
    using namespace std;
    ostringstream chunk(ios::binary);
    unsigned int curTotal = 1;
    int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
    char lastId = worldArray[0];
    for(int i = 1; i<numbOfBlocks;i++)
    {
      if(lastId == worldArray[i])
      {
        curTotal++;
      }
      else
      {
        chunk.write((char*) &lastId,sizeof(lastId));
        chunk.write((char*) &curTotal,sizeof(curTotal));
        curTotal = 1;
        lastId = worldArray[i];
      }
    }
    chunk.write((char*) &lastId,sizeof(lastId));
    chunk.write((char*) &curTotal,sizeof(curTotal));
    return chunk.str();

}

inline void BSP::saveChunk()
{
  using namespace std;
  //The directoy to the chunk to be saved
  string directory = "saves/" + worldName + "/chunks/";
  string chunkName = to_string(xCoord) + '_' + to_string(yCoord) + '_' + to_string(zCoord) + ".dat";
  string chunkPath = directory+chunkName;

  //Max size of a chunk
  int numbOfBlocks = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
  ofstream ochunk(chunkPath,ios::binary);
  unsigned int curTotal = 1;
  char lastId = worldArray[0];

  for(int i = 1; i<numbOfBlocks;i++)
  {
    if(lastId == worldArray[i])
    {
      curTotal++;
    }
    else
    {
      ochunk.write((char*) &lastId,sizeof(lastId));
      ochunk.write((char*) &curTotal,sizeof(curTotal));
      curTotal = 1;
      lastId = worldArray[i];
    }
  }
  ochunk.write((char*) &lastId,sizeof(lastId));
  ochunk.write((char*) &curTotal,sizeof(curTotal));
  ochunk.close();
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

inline int pack4chars(char a, char b, char c, char d)
{
  return ((a << 24) | (b << 16) | (c << 8) | d);
}

inline int BSP::addVertex(int renderType,const glm::vec3 &pos,Faces face, TextureSides top, TextureSides right)
{
  if(renderType == 0)
  {
    int numbVert = oVerticesBuffer->size()/4;
    //Adds position vector
    oVerticesBuffer->push_back(pos.x);
    oVerticesBuffer->push_back(pos.y);
    oVerticesBuffer->push_back(pos.z);

    char normandtex = face | top | right;
    char texId = curBlockid;
    char xtextcount = xdist;
    char ytextcount = ydist;

    //std::cout << "Norm is" << face << "\n";
    int package = pack4chars(normandtex,texId,xtextcount,ytextcount);
    oVerticesBuffer->push_back(*(float*)&package);
    /*
    //Adds normal vector
    oVerticesBuffer->push_back(norm.x);
    oVerticesBuffer->push_back(norm.y);
    oVerticesBuffer->push_back(norm.z);

    //Adds the textureCoordinate
    oVerticesBuffer->push_back(texX);
    oVerticesBuffer->push_back(texY);
    //Returns the location of the vertice
    */
    return numbVert;
  }
  /*
  else if(renderType == 1)
  {
    int numbVert = tVerticesBuffer->size()/8;
    //Adds position vector
    tVerticesBuffer->push_back(pos.x);
    tVerticesBuffer->push_back(pos.y);
    tVerticesBuffer->push_back(pos.z);

    //Adds normal vector
    tVerticesBuffer->push_back(norm.x);
    tVerticesBuffer->push_back(norm.y);
    tVerticesBuffer->push_back(norm.z);

    //Adds the textureCoordinate
    tVerticesBuffer->push_back(texX);
    tVerticesBuffer->push_back(texY);
    //Returns the location of the vertice
    return numbVert;
  }
  */
}

inline void BSP::addIndices(int renderType,int index1, int index2, int index3, int index4)
{
  if(renderType == 0)
  {
    oIndicesBuffer->push_back(index1);
    oIndicesBuffer->push_back(index2);
    oIndicesBuffer->push_back(index3);

    oIndicesBuffer->push_back(index2);
    oIndicesBuffer->push_back(index4);
    oIndicesBuffer->push_back(index3);
  }
  else if(renderType == 1)
  {
    tIndicesBuffer->push_back(index1);
    tIndicesBuffer->push_back(index2);
    tIndicesBuffer->push_back(index3);

    tIndicesBuffer->push_back(index2);
    tIndicesBuffer->push_back(index4);
    tIndicesBuffer->push_back(index3);
  }
}


inline bool BSP::blockExists(int x, int y, int z)
{
  return worldArray.get(x,y,z) == 0 ? false : true;
}

inline int BSP::blockVisibleType(int x, int y, int z)
{
  return ItemDatabase::blockDictionary[getBlock(x,y,z)].visibleType;
}

void BSP::addBlock(int x, int y, int z, char id)
{
  worldArray.set(x,y,z,id);
}

inline void BSP::delBlock(int x, int y, int z)
{
  worldArray.set(x,y,z,0);

}

inline uchar BSP::getBlock(int x, int y, int z)
{
  return worldArray.get(x,y,z);
}

inline glm::vec3 BSP::offset(float x, float y, float z)
{
  /*
  glm::vec3 newVec;
  long long int id = x*y*z+seed*32;
  newVec.x = id % (12355 % 1000-500)/(float)500 + x;
  newVec.y = id % (23413 % 1000-500)/(float)500 + y;
  newVec.z = id % (14351 % 1000-500)/(float)500 + z;
  */
  return glm::vec3(x,y,z);

}



void BSP::build(std::shared_ptr<BSPNode>  curRightChunk,std::shared_ptr<BSPNode>  curLeftChunk,std::shared_ptr<BSPNode>  curTopChunk,
                       std::shared_ptr<BSPNode>  curBottomChunk,std::shared_ptr<BSPNode>  curFrontChunk,std::shared_ptr<BSPNode>  curBackChunk)
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
         int renderType = blockVisibleType(x,y,z);

         float realX = x+CHUNKSIZE*xCoord;
         float realY = y+CHUNKSIZE*yCoord;
         float realZ = z+CHUNKSIZE*zCoord;

         bool topNeigh = false;
         bool bottomNeigh = false;
         bool leftNeigh = false;
         bool rightNeigh = false;
         bool frontNeigh = false;
         bool backNeigh = false;

         bool defaultNull = false;

         if(x+1 >= CHUNKSIZE)
         {
           if(curRightChunk != NULL && renderType == curRightChunk->blockVisibleType(0,y,z) )
           {
             rightNeigh = true;
           }
           else if(defaultNull) rightNeigh = true;
         }
         else if(renderType == blockVisibleType(x+1,y,z)) rightNeigh = true;

         if(x-1 < 0)
         {
           if(curLeftChunk != NULL && renderType == curLeftChunk->blockVisibleType(CHUNKSIZE-1,y,z))
           {
             leftNeigh = true;
           }
           else if(defaultNull) leftNeigh = true;
         }
         else if(renderType == blockVisibleType(x-1,y,z)) leftNeigh = true;

         if(y+1 >= CHUNKSIZE)
         {
           if(curTopChunk != NULL && renderType == curTopChunk->blockVisibleType(x,0,z))
           {
              topNeigh = true;
           }
           else if(defaultNull) topNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y+1,z)) topNeigh = true;

         if(y-1 < 0)
         {
          if(curBottomChunk != NULL && renderType == curBottomChunk->blockVisibleType(x,CHUNKSIZE-1,z))
          {
            bottomNeigh = true;
          }
          else if(defaultNull) bottomNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y-1,z)) bottomNeigh = true;

         if(z+1 >= CHUNKSIZE)
         {
           if(curBackChunk != NULL && renderType == curBackChunk->blockVisibleType(x,y,0))
           {
             backNeigh = true;
           }
           else if(defaultNull) backNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y,z+1)) backNeigh = true;

         if(z-1 < 0)
         {
           if(curFrontChunk != NULL && renderType == curFrontChunk->blockVisibleType(x,y,CHUNKSIZE-1))
           {
             frontNeigh = true;
           }
           else if(defaultNull) frontNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y,z-1)) frontNeigh = true;

         BlockFace& curFace = arrayFaces.get(x,y,z);
         if(!frontNeigh) curFace.setFace(FRONTF);
         if(!backNeigh) curFace.setFace(BACKF);
         if(!topNeigh) curFace.setFace(TOPF);
         if(!bottomNeigh) curFace.setFace(BOTTOMF);
         if(!leftNeigh) curFace.setFace(LEFTF);
         if(!rightNeigh) curFace.setFace(RIGHTF);

       }
     }
   }

   for(int x = 0; x<CHUNKSIZE;x++)
   {
     for(int z = 0;z<CHUNKSIZE;z++)
     {
       for(int y = 0;y<CHUNKSIZE;y++)
       {
         int renderType = blockVisibleType(x,y,z);
         float realX = x+CHUNKSIZE*xCoord;
         float realY = y+CHUNKSIZE*yCoord;
         float realZ = z+CHUNKSIZE*zCoord;


         BlockFace curFace = arrayFaces.get(x,y,z);
         char blockId = getBlock(x,y,z);
         Block tempBlock = ItemDatabase::blockDictionary[blockId];

         glm::vec3 topleft, bottomleft,topright,bottomright;
         glm::vec3 tempVec;
         Faces normVec;

         xdist = 1;
         ydist = 1;
         if(curFace.getFace(TOPF))
         {
           char top = 1;
           char right = 1;

           arrayFaces.get(x,y,z).delFace(TOPF);
           while(x+right < CHUNKSIZE && getBlock(x+right,y,z) == blockId &&
                 arrayFaces.get(x+right,y,z).getFace(TOPF))
           {
             arrayFaces.get(x+right,y,z).delFace(TOPF);
             right +=1;
           }

           while(z+top < CHUNKSIZE && getBlock(x,y,z+top) == blockId
                 && arrayFaces.get(x,y,z+top).getFace(TOPF))
           {
             bool clearRow = true;
             for(int i=0;i<right;i++)
             {
               if(getBlock(x+i,y,z+top) != blockId || !arrayFaces.get(x+i,y,z+top).getFace(TOPF) ) clearRow = false;
             }

             if(clearRow)
             {
               for(int i=0;i<right;i++)
               {
                 arrayFaces.get(x+i,y,z+top).delFace(TOPF);
               }
               top += 1;
             }
             else break;
           }


           bottomleft = glm::vec3(realX+right,realY+1.0f,realZ);
           bottomright = glm::vec3(realX+right,realY+1.0f,realZ+top);
           topleft = glm::vec3(realX,realY+1.0f,realZ);
           topright = glm::vec3(realX,realY+1.0f,realZ+top);

           normVec = TOPF;
           xdist = right;
           ydist = top;
           curBlockid = tempBlock.getTop();

           int index1 = addVertex(renderType,topleft,normVec,TOP,LEFT);
           int index3 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT);
           int index2 = addVertex(renderType,topright,normVec,TOP,RIGHT);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT);


           addIndices(renderType,index1,index2,index3,index4);
         }

         if(curFace.getFace(BOTTOMF))
         {
           char top = 1;
           char right = 1;

           arrayFaces.get(x,y,z).delFace(BOTTOMF);


           while(x+right < CHUNKSIZE && getBlock(x+right,y,z) == blockId &&
                 arrayFaces.get(x+right,y,z).getFace(BOTTOMF))
           {
             arrayFaces.get(x+right,y,z).delFace(BOTTOMF);
             right +=1;
           }

           while(z+top < CHUNKSIZE && getBlock(x,y,z+top) == blockId
                 && arrayFaces.get(x,y,z+top).getFace(BOTTOMF))
           {
             bool clearRow = true;
             for(int i=0;i<right;i++)
             {
               if(getBlock(x+i,y,z+top) != blockId || !arrayFaces.get(x+i,y,z+top).getFace(BOTTOMF) ) clearRow = false;
             }

             if(clearRow)
             {
               for(int i=0;i<right;i++)
               {
                 arrayFaces.get(x+i,y,z+top).delFace(BOTTOMF);
               }
               top += 1;
             }
             else break;
           }

           topleft = glm::vec3(realX,realY,realZ);
           bottomleft = glm::vec3(realX+right,realY,realZ);
           topright = glm::vec3(realX,realY,realZ+top);
           bottomright = glm::vec3(realX+right,realY,realZ+top);
           normVec = BOTTOMF;
           curBlockid = tempBlock.getBottom();
           xdist = right;
           ydist = top;
           int index1 = addVertex(renderType,topleft,normVec,TOP,LEFT);
           int index2 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT);
           int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT);

           addIndices(renderType,index1,index2,index3,index4);
         }

         if(curFace.getFace(RIGHTF))
         {
           char top = 1;
           char right = 1;
           arrayFaces.get(x,y,z).delFace(RIGHTF);

           while(z+right < CHUNKSIZE && getBlock(x,y,z+right) == blockId
                 && arrayFaces.get(x,y,z+right).getFace(RIGHTF))
           {
             arrayFaces.get(x,y,z+right).delFace(RIGHTF);
             right +=1;
           }

           while(y+top < CHUNKSIZE && getBlock(x,y+top,z) == blockId
                 && arrayFaces.get(x,y+top,z).getFace(RIGHTF))
           {
             bool clearRow = true;
             for(int i=0;i<right;i++)
             {
               if(getBlock(x,y+top,z+i) != blockId || !arrayFaces.get(x,y+top,z+i).getFace(RIGHTF)) clearRow = false;
             }

             if(clearRow)
             {
               for(int i=0;i<right;i++)
               {
                 arrayFaces.get(x,y+top,z+i).delFace(RIGHTF);
               }
               top += 1;
             }
             else break;
           }
           topleft = glm::vec3(realX+1.0f,realY,realZ);
           bottomleft = glm::vec3(realX+1.0f,realY+top,realZ);
           topright = glm::vec3(realX+1.0f,realY,realZ+right);
           bottomright = glm::vec3(realX+1.0f,realY+top,realZ +right);
           normVec = RIGHTF;
           xdist = top;
           ydist = right;
           curBlockid = tempBlock.getRight();
           int index1 = addVertex(renderType,topleft,normVec,TOP,LEFT);
           int index2 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT);
           int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT);

           addIndices(renderType,index1,index2,index3,index4);
         }

         if(curFace.getFace(LEFTF))
         {
           char top = 1;
           char right = 1;
           arrayFaces.get(x,y,z).delFace(LEFTF);

           while(z+right < CHUNKSIZE && getBlock(x,y,z+right) == blockId
                 && arrayFaces.get(x,y,z+right).getFace(LEFTF))
           {
             arrayFaces.get(x,y,z+right).delFace(LEFTF);
             right +=1;
           }

           while(y+top < CHUNKSIZE && getBlock(x,y+top,z) == blockId
                 && arrayFaces.get(x,y+top,z).getFace(LEFTF))
           {
             bool clearRow = true;
             for(int i=0;i<right;i++)
             {
               if(getBlock(x,y+top,z+i) != blockId || !arrayFaces.get(x,y+top,z+i).getFace(LEFTF)) clearRow = false;
             }

             if(clearRow)
             {
               for(int i=0;i<right;i++)
               {
                 arrayFaces.get(x,y+top,z+i).delFace(LEFTF);
               }
               top += 1;
             }
             else break;
           }


           topleft = glm::vec3(realX,realY,realZ);
           bottomleft = glm::vec3(realX,realY+top,realZ);
           topright = glm::vec3(realX,realY,realZ+right);
           bottomright = glm::vec3(realX,realY+top,realZ +right);
           normVec = LEFTF;
           xdist = top;
           ydist = right;
           curBlockid = tempBlock.getLeft();
           int index1 = addVertex(renderType,topleft,normVec,TOP,LEFT);
           int index3 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT);
           int index2 = addVertex(renderType,topright,normVec,TOP,RIGHT);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT);

           addIndices(renderType,index1,index2,index3,index4);
         }



         if(curFace.getFace(BACKF))
         {
           char top = 1;
           char right = 1;

           arrayFaces.get(x,y,z).delFace(BACKF);


           while(x+right < CHUNKSIZE && getBlock(x+right,y,z) == blockId
                 && arrayFaces.get(x+right,y,z).getFace(BACKF))
           {
             arrayFaces.get(x+right,y,z).delFace(BACKF);
             right +=1;
           }

           while(y+top < CHUNKSIZE && getBlock(x,y+top,z) == blockId
                 && arrayFaces.get(x,y+top,z).getFace(BACKF))
           {
             bool clearRow = true;
             for(int i=0;i<right;i++)
             {
               if(getBlock(x+i,y+top,z) != blockId || !arrayFaces.get(x+i,y+top,z).getFace(BACKF)) clearRow = false;
             }

             if(clearRow)
             {
               for(int i=0;i<right;i++)
               {
                 arrayFaces.get(x+i,y+top,z).delFace(BACKF);
               }
               top += 1;
             }
             else break;
           }

           topleft = glm::vec3(realX,realY,realZ+1.0f);
           bottomleft = glm::vec3(realX+right,realY,realZ+1.0f);
           topright = glm::vec3(realX,realY+top,realZ+1.0f);
           bottomright = glm::vec3(realX+right,realY+top,realZ+1.0f);

           curBlockid = tempBlock.getBack();
           normVec = BACKF;
           xdist = right;
           ydist = top;
           int index1 = addVertex(renderType,topleft,normVec,TOP,LEFT);
           int index2 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT);
           int index3 = addVertex(renderType,topright,normVec,TOP,RIGHT);
           int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT);
           addIndices(renderType,index1,index2,index3,index4);
         }


         if(curFace.getFace(FRONTF))
          {
            char top = 1;
            char right = 1;

            arrayFaces.get(x,y,z).delFace(FRONTF);


            while(x+right < CHUNKSIZE && getBlock(x+right,y,z) == blockId &&
                  arrayFaces.get(x+right,y,z).getFace(FRONTF))
            {
              arrayFaces.get(x+right,y,z).delFace(FRONTF);
              right +=1;
            }

            while(y+top < CHUNKSIZE && getBlock(x,y+top,z) == blockId
                  && arrayFaces.get(x,y+top,z).getFace(FRONTF))
            {
              bool clearRow = true;
              for(int i=0;i<right;i++)
              {
                if(getBlock(x+i,y+top,z) != blockId || !arrayFaces.get(x+i,y+top,z).getFace(FRONTF) ) clearRow = false;
              }

              if(clearRow)
              {
                for(int i=0;i<right;i++)
                {
                  arrayFaces.get(x+i,y+top,z).delFace(FRONTF);
                }
                top += 1;
              }
              else break;
            }

            xdist = right;
            ydist = top;
            topleft = glm::vec3(realX,realY,realZ);
            bottomleft = glm::vec3(realX+right,realY,realZ);
            topright = glm::vec3(realX,realY+top,realZ);
            bottomright = glm::vec3(realX+right,realY+top,realZ);

            curBlockid = tempBlock.getBack();
            normVec = FRONTF;


            int index1 = addVertex(renderType,topleft,normVec,TOP,LEFT);
            int index3 = addVertex(renderType,bottomleft,normVec,BOTTOM,LEFT);
            int index2 = addVertex(renderType,topright,normVec,TOP,RIGHT);
            int index4 = addVertex(renderType,bottomright,normVec,BOTTOM,RIGHT);

            addIndices(renderType,index1,index2,index3,index4);

          }
       }
     }
   }

  oVertices = oVerticesBuffer;
  oIndices = oIndicesBuffer;
  tVertices = tVerticesBuffer;
  tIndices = tIndicesBuffer;

  oVerticesBuffer = std::shared_ptr<std::vector<GLfloat>> (new std::vector<GLfloat>);
  oIndicesBuffer  = std::shared_ptr<std::vector<GLuint>> (new std::vector<GLuint>);
  tVerticesBuffer = std::shared_ptr<std::vector<GLfloat>> (new std::vector<GLfloat>);
  tIndicesBuffer  = std::shared_ptr<std::vector<GLuint>> (new std::vector<GLuint>);

}

inline void BSP::render()
{
  if(oIndices->size() != 0)
  {
  glDeleteBuffers(1, &oVBO);
  glDeleteBuffers(1, &oEBO);
  glDeleteVertexArrays(1, &oVAO);


  glGenVertexArrays(1, &oVAO);
  glGenBuffers(1, &oEBO);
  glGenBuffers(1, &oVBO);
  glBindVertexArray(oVAO);

  glBindBuffer(GL_ARRAY_BUFFER,oVBO);
  glBufferData(GL_ARRAY_BUFFER, oVertices->size()*sizeof(float),&oVertices->front(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,oEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, oIndices->size()*sizeof(uint),&oIndices->front(), GL_STATIC_DRAW);

  int vertexSize = 4*sizeof(float);
  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,vertexSize, (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,vertexSize, (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
    geometryChanged = true;
  }


  if(tIndices->size() != 0)
  {
  std::cout << "ITS BROKE\n";
  glDeleteBuffers(1, &tVBO);
  glDeleteBuffers(1, &tEBO);
  glDeleteVertexArrays(1, &tVAO);

  glGenVertexArrays(1, &tVAO);
  glGenBuffers(1, &tEBO);
  glGenBuffers(1, &tVBO);
  glBindVertexArray(tVAO);

  glBindBuffer(GL_ARRAY_BUFFER,tVBO);
  glBufferData(GL_ARRAY_BUFFER, tVertices->size()*sizeof(float),&tVertices->front(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,tEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, tIndices->size()*sizeof(uint),&tIndices->front(), GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  }


}

void BSP::drawOpaque()
{

  if(oIndices->size() != 0)
  {
    //drawnChunks++;
    glBindVertexArray(oVAO);
    glDrawElements(GL_TRIANGLES, oIndices->size(), GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
  }
}

void BSP::drawTranslucent()
{
  glBindVertexArray(tVAO);
  glDrawElements(GL_TRIANGLES, tIndices->size(), GL_UNSIGNED_INT,0);
  glBindVertexArray(0);
}

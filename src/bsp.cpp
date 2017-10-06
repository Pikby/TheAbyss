
#include "headers/all.h"

int totalChunks;


BSPNode::BSPNode(long int x, long int y, long int z)
{
  //std::cout << totalChunks << "\n";
  curBSP = BSP(x,y,z);

  isGenerated = false;
  nextNode = NULL;
  prevNode = NULL;
  inUse = false;
  toRender = false;
  toBuild = false;
  toDelete = false;
  totalChunks++;
}

BSPNode::~BSPNode()
{
  totalChunks--;
}

void BSPNode::generateTerrain()
{
  curBSP.generateTerrain();
}

void BSPNode::build()
{
  while(inUse)
  {
    std::cout << "build waiting\n";
    if(toDelete == true) return;
  }
  inUse = true;
  curBSP.build(rightChunk,leftChunk,topChunk,bottomChunk,frontChunk,backChunk);
  toRender = true;
  toBuild = false;
  inUse = false;
}

void BSPNode::drawOpaque(Camera* camera)
{
  inUse = true;
  if(toRender == true)
  {
     curBSP.render();
     toRender = false;
  }
  curBSP.drawOpaque(camera);
  inUse = false;
}

void BSPNode::drawTranslucent(Camera* camera)
{

  inUse = true;
  if(toRender == true)
  {
     curBSP.render();
     toRender = false;
  }
  curBSP.drawTranslucent(camera);
  inUse = false;
}

bool BSPNode::blockExists(int x, int y, int z)
{
  return curBSP.blockExists(x, y, z);
}

int BSPNode::blockVisibleType(int x, int y, int z)
{
  return curBSP.blockVisibleType(x, y, z);
}
BSP::BSP(long int x, long int y, long int z)
{
    xCoord = x;
    yCoord = y;
    zCoord = z;
    for(int x = 0;x<CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;x++) worldMap[x] = 0;

    using namespace std;

    string directory = "saves/" + worldName + "/chunks/";
    string chunkName = to_string(x) + '_'+to_string(y) + '_' + to_string(z) + ".dat";
    string chunkPath = directory+chunkName;
    int numbOfInts = CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;
    ifstream ichunk(chunkPath,ios::binary);
    if(!ichunk.is_open())
    {
      generateTerrain();
      ichunk.close();
      ofstream ochunk(chunkPath, ios::binary);
      for(int i = 0 ;i<numbOfInts;i++)
      {
        ochunk << worldMap[i];
      }

      ochunk.close();
    }
    else
    {
      for(int i = 0;i<numbOfInts;i++)
      {
        ichunk >> worldMap[i];
      }
      ichunk.close();
    }

    oVerticesBuffer = std::shared_ptr<std::vector<GLfloat>> (new std::vector<GLfloat>);
    oIndicesBuffer  = std::shared_ptr<std::vector<GLuint>> (new std::vector<GLuint>);
    tVerticesBuffer = std::shared_ptr<std::vector<GLfloat>> (new std::vector<GLfloat>);
    tIndicesBuffer  = std::shared_ptr<std::vector<GLuint>> (new std::vector<GLuint>);

    oVertices = std::shared_ptr<std::vector<GLfloat>> (new std::vector<GLfloat>);
    oIndices  = std::shared_ptr<std::vector<GLuint>> (new std::vector<GLuint>);
    tVertices = std::shared_ptr<std::vector<GLfloat>> (new std::vector<GLfloat>);
    tIndices = std::shared_ptr<std::vector<GLuint>> (new std::vector<GLuint>);


}

BSP::~BSP()
{

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

BSP::BSP(){}





int BSP::addVertex(int renderType,float x, float y, float z, float xn, float yn, float zn, float texX, float texY)
{
  if(renderType == OPAQUE)
  {
    int numbVert = oVerticesBuffer->size()/8;
    //Adds position vector
    oVerticesBuffer->push_back(x);
    oVerticesBuffer->push_back(y);
    oVerticesBuffer->push_back(z);

    //Adds normal vector
    oVerticesBuffer->push_back(xn);
    oVerticesBuffer->push_back(yn);
    oVerticesBuffer->push_back(zn);

    //Adds the textureCoordinate
    oVerticesBuffer->push_back(texX);
    oVerticesBuffer->push_back(texY);
    //Returns the location of the vertice
    return numbVert;
  }
  else if(renderType == TRANSLUCENT)
  {
    int numbVert = tVerticesBuffer->size()/8;
    //Adds position vector
    tVerticesBuffer->push_back(x);
    tVerticesBuffer->push_back(y);
    tVerticesBuffer->push_back(z);

    //Adds normal vector
    tVerticesBuffer->push_back(xn);
    tVerticesBuffer->push_back(yn);
    tVerticesBuffer->push_back(zn);

    //Adds the textureCoordinate
    tVerticesBuffer->push_back(texX);
    tVerticesBuffer->push_back(texY);
    //Returns the location of the vertice
    return numbVert;
  }
}

void BSP::addIndices(int renderType,int index1, int index2, int index3, int index4)
{
  if(renderType == OPAQUE)
  {
    oIndicesBuffer->push_back(index1);
    oIndicesBuffer->push_back(index2);
    oIndicesBuffer->push_back(index3);

    oIndicesBuffer->push_back(index2);
    oIndicesBuffer->push_back(index4);
    oIndicesBuffer->push_back(index3);
  }
  else if(renderType == TRANSLUCENT)
  {
    tIndicesBuffer->push_back(index1);
    tIndicesBuffer->push_back(index2);
    tIndicesBuffer->push_back(index3);

    tIndicesBuffer->push_back(index2);
    tIndicesBuffer->push_back(index4);
    tIndicesBuffer->push_back(index3);
  }
}


bool BSP::blockExists(int x, int y, int z)
{
  if(worldMap[x+CHUNKSIZE*y+z*CHUNKSIZE*CHUNKSIZE] == 0) return false;
  else return true;
}

int BSP::blockVisibleType(int x, int y, int z)
{
  return dictionary[getBlock(x,y,z)]->visibleType;
}

void BSP::addBlock(int x, int y, int z, char id)
{
  worldMap[x+y*CHUNKSIZE+z*CHUNKSIZE*CHUNKSIZE] = id;
}

int BSP::removeBlock(int x, int y, int z)
{
  worldMap[x + y*CHUNKSIZE + z*CHUNKSIZE*CHUNKSIZE] = 0;
}

int BSP::getBlock(int x, int y, int z)
{
  return worldMap[x+y*CHUNKSIZE+z*CHUNKSIZE*CHUNKSIZE];
}

glm::vec3 BSP::offset(float x, float y, float z)
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

  for(int x = 0; x<CHUNKSIZE;x++)
  {
     for(int y = 0;y<CHUNKSIZE;y++)
     {
       for(int z = 0;z<CHUNKSIZE;z++)
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
         bool defaultNull = true;

         if(x+1 >= CHUNKSIZE)
         {
           if(curRightChunk != NULL)
           {
              if(renderType == curRightChunk->blockVisibleType(0,y,z)) rightNeigh = true;
           }
           else if(defaultNull) rightNeigh = true;
         }
         else if(renderType == blockVisibleType(x+1,y,z)) rightNeigh = true;

         if(x-1 < 0)
         {
           if(curLeftChunk != NULL)
           {
            if(renderType == curLeftChunk->blockVisibleType(CHUNKSIZE-1,y,z)) leftNeigh = true;
           }
           else if(defaultNull) leftNeigh = true;
         }
         else if(renderType == blockVisibleType(x-1,y,z)) leftNeigh = true;

         if(y+1 >= CHUNKSIZE)
         {
           if(curTopChunk != NULL)
           {
              if(renderType == curTopChunk->blockVisibleType(x,0,z)) topNeigh = true;
           }
           else if(defaultNull) topNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y+1,z)) topNeigh = true;

         if(y-1 < 0)
         {
          if(curBottomChunk != NULL)
          {
            if(renderType == curBottomChunk->blockVisibleType(x,CHUNKSIZE-1,z)) bottomNeigh = true;
          }
          else if(defaultNull) bottomNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y-1,z)) bottomNeigh = true;

         if(z+1 >= CHUNKSIZE)
         {
           if(curBackChunk != NULL)
           {
             if(renderType == curBackChunk->blockVisibleType(x,y,0)) backNeigh = true;
           }
           else if(defaultNull) backNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y,z+1)) backNeigh = true;

         if(z-1 < 0)
         {
           if(curFrontChunk != NULL)
           {
             if(renderType == curFrontChunk->blockVisibleType(x,y,CHUNKSIZE-1)) frontNeigh = true;
           }
           else if(defaultNull) frontNeigh = true;
         }
         else if(renderType == blockVisibleType(x,y,z-1)) frontNeigh = true;

         Block* tempBlock = dictionary[(getBlock(x,y,z))];
         if(tempBlock == NULL) return;
         float x1, y1, x2, y2;

         glm::vec3 tempVec;
         if(!topNeigh)
         {
           tempBlock->getTop(&x1,&y1,&x2,&y2);
           tempVec = offset(realX,realY+1.0f,realZ);
           int index1 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,1.0f,0.0f,x1,y1);

           tempVec = offset(realX+1.0f,realY+1.0f,realZ);
           int index3 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,1.0f,0.0f,x2,y1);

           tempVec = offset(realX,realY+1.0f,realZ+1.0f);
           int index2 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,1.0f,0.0f,x1,y2);

           tempVec = offset(realX+1.0f,realY+1.0f,realZ+1.0f);
           int index4 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,1.0f,0.0f,x2,y2);

           addIndices(renderType,index1,index2,index3,index4);
         }

         if(!bottomNeigh)
         {
           tempBlock->getBottom(&x1,&y1,&x2,&y2);

           tempVec = offset(realX,realY,realZ);
           int index1 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,-1.0f,0.0f,x1,y1);

           tempVec = offset(realX+1.0f,realY,realZ);
           int index2 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,-1.0f,0.0f,x2,y1);

           tempVec = offset(realX,realY,realZ+1.0f);
           int index3 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,-1.0f,0.0f,x1,x2);

           tempVec = offset(realX+1.0f,realY,realZ+1.0f);
           int index4 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,-1.0f,0.0f,x2,x2);

           addIndices(renderType,index1,index2,index3,index4);
         }

         if(!rightNeigh)
         {
           tempBlock->getRight(&x1,&y1,&x2,&y2);

           tempVec = offset(realX+1.0f,realY,realZ);
           int index1 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,1.0f,0.0f,0.0f,x1,y1);

           tempVec = offset(realX+1.0f,realY+1.0f,realZ);
           int index2 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,1.0f,0.0f,0.0f,x2,y1);

           tempVec = offset(realX+1.0f,realY,realZ+1.0f);
           int index3 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,1.0f,0.0f,0.0f,x1,y2);

           tempVec = offset(realX+1.0f,realY+1.0f,realZ+1.0f);
           int index4 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,1.0f,0.0f,0.0f,x2,y2);

           addIndices(renderType,index1,index2,index3,index4);
         }

         if(!leftNeigh)
         {
           tempBlock->getLeft(&x1,&y1,&x2,&y2);

           tempVec = offset(realX,realY,realZ);
           int index1 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,-1.0f,0.0f,0.0f,x1,y1);
           tempVec = offset(realX,realY+1.0f,realZ);
           int index3 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,-1.0f,0.0f,0.0f,x2,y1);
           tempVec = offset(realX,realY,realZ+1.0f);
           int index2 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,-1.0f,0.0f,0.0f,x1,y2);
           tempVec = offset(realX,realY+1.0f,realZ +1.0f);
           int index4 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,-1.0f,0.0f,0.0f,x2,y2);

           addIndices(renderType,index1,index2,index3,index4);
         }
         if(!backNeigh)
         {
           tempBlock->getBack(&x1,&y1,&x2,&y2);

           tempVec = offset(realX,realY,realZ+1.0f);
           int index1 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,0.0f,1.0f,x1,y1);
           tempVec = offset(realX+1.0f,realY,realZ+1.0f);
           int index2 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,0.0f,1.0f,x2,y1);
           tempVec = offset(realX,realY+1.0f,realZ+1.0f);
           int index3 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,0.0f,1.0f,x1,y2);
           tempVec = offset(realX+1.0f,realY+1.0f,realZ+1.0f);
           int index4 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,0.0f,1.0f,x2,y2);

           addIndices(renderType,index1,index2,index3,index4);
         }

         if(!frontNeigh)
         {
           tempBlock->getFront(&x1,&y1,&x2,&y2);
           tempVec = offset(realX,realY,realZ);
           int index1 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,0.0f,-1.0f,x1,y1);
           tempVec = offset(realX+1.0f,realY,realZ);
           int index3 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,0.0f,-1.0f,x2,y1);
           tempVec = offset(realX,realY+1.0f,realZ);
           int index2 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,0.0f,-1.0f,x1,y2);
           tempVec = offset(realX+1.0f,realY+1.0f,realZ);
           int index4 = addVertex(renderType,tempVec.x, tempVec.y, tempVec.z,0.0f,0.0f,-1.0f,x2,y2);
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

void BSP::render()
{
  glDeleteBuffers(1, &oVBO);
  glDeleteBuffers(1, &oEBO);
  glDeleteVertexArrays(1, &oVAO);


  glGenVertexArrays(1, &oVAO);
  glGenBuffers(1, &oEBO);
  glGenBuffers(1, &oVBO);
  glBindVertexArray(oVAO);

  glBindBuffer(GL_ARRAY_BUFFER,oVBO);
  glBufferData(GL_ARRAY_BUFFER, oVertices->size()*sizeof(GLfloat),&oVertices->front(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,oEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, oIndices->size()*sizeof(GLuint),&oIndices->front(), GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteBuffers(1, &tVBO);
  glDeleteBuffers(1, &tEBO);
  glDeleteVertexArrays(1, &tVAO);

  glGenVertexArrays(1, &tVAO);
  glGenBuffers(1, &tEBO);
  glGenBuffers(1, &tVBO);
  glBindVertexArray(tVAO);

  glBindBuffer(GL_ARRAY_BUFFER,tVBO);
  glBufferData(GL_ARRAY_BUFFER, tVertices->size()*sizeof(GLfloat),&tVertices->front(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,tEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, tIndices->size()*sizeof(GLuint),&tIndices->front(), GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


}

void BSP::drawOpaque(Camera* camera)
{
  glBindVertexArray(oVAO);
  glDrawElements(GL_TRIANGLES, oIndices->size(), GL_UNSIGNED_INT,0);
  glBindVertexArray(0);
}

void BSP::drawTranslucent(Camera* camera)
{
  glBindVertexArray(tVAO);
  glDrawElements(GL_TRIANGLES, tIndices->size(), GL_UNSIGNED_INT,0);
  glBindVertexArray(0);
}

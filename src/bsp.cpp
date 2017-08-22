


#include <string>
#include "headers/SOIL.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <thread>
#include <queue>

// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

//Add the shader configs
#include "headers/shaders.h"
#include "headers/camera.h"
#include "headers/bsp.h"
int totalChunks;


BSPNode::BSPNode(Shader* shader, Block** dict, GLuint* newglTexture, long int x, long int y, long int z,  siv::PerlinNoise* perlin)
{
  curBSP = BSP(shader,dict,newglTexture,x,y,z,perlin);
  curBSP.generateTerrain(perlin);

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
  std::cout << "deleting chunk\n";
  totalChunks--;
}


void BSPNode::build(World* curWorld)
{
  std::cout << totalChunks << "\n";
  while(inUse)
  {
    if(toDelete == true) return;
  }
  inUse = true;
  curBSP.build(curWorld,rightChunk,leftChunk,topChunk,bottomChunk,frontChunk,backChunk);
  toRender = true;
  inUse = false;
}

void BSPNode::draw(Camera* camera,float lightposx,float lightposy,float lightposz)
{
  if(inUse)
  {
    return;
  }

  inUse = true;
  if(toRender == true)
  {
     curBSP.render();
     toRender = false;
  }
  curBSP.draw(camera,lightposx,lightposy,lightposz);
  inUse = false;
}

bool BSPNode::blockExists(int x, int y, int z)
{
  if(toDelete == true) return false;
  return curBSP.blockExists(x, y, z);
}

BSP::BSP(Shader* shader, Block** dict, GLuint* newglTexture, long int x, long int y, long int z,  siv::PerlinNoise* perlin)
{
    xCoord = x;
    yCoord = y;
    zCoord = z;
    blockShader = shader;
    dictionary = dict;
    glTexture = newglTexture;
    for(int x = 0;x<CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;x++) worldMap[x] = 0;


}

BSP::~BSP()
{

}
void BSP::freeGL()
{
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&EBO);
    glDeleteVertexArrays(1,&VAO);
}

BSP::BSP(){}


void BSP::generateTerrain(siv::PerlinNoise* perlin)
{
  double freq = 128;
  int oct = 8;


  for(int x=0;x<CHUNKSIZE;x++)
  {
    for(int z=0;z<CHUNKSIZE;z++)
    {
      int height = CHUNKSIZE+200*perlin->octaveNoise0_1((x+xCoord*CHUNKSIZE)/freq,(z+zCoord*CHUNKSIZE)/freq,oct);
      for(int y=0;y<CHUNKSIZE;y++)
      {

        if(yCoord*CHUNKSIZE+y <height)
        {
          if(yCoord*CHUNKSIZE+y == height - 1) addBlock(x,y,z,2);
          else addBlock(x,y,z,1);
        }
      }
    }
  }
}


int BSP::addVertex(float x, float y, float z, float xn, float yn, float zn, float texX, float texY)
{
  int numbVert = vertices.size()/8;

  vertices.push_back(x);
  vertices.push_back(y);
  vertices.push_back(z);

  vertices.push_back(xn);
  vertices.push_back(yn);
  vertices.push_back(zn);

  vertices.push_back(texX);
  vertices.push_back(texY);
  return numbVert;
}

bool BSP::blockExists(int x, int y, int z)
{
  if(x>=CHUNKSIZE || y >=CHUNKSIZE || z >= CHUNKSIZE || x<0 || y<0 || z<0) return false ;
  if(worldMap[x+CHUNKSIZE*y+z*CHUNKSIZE*CHUNKSIZE] == 0) return false;
  else return true;
}

void BSP::addIndices(int index1, int index2, int index3, int index4)
{
  indices.push_back(index1);
  indices.push_back(index2);
  indices.push_back(index3);

  indices.push_back(index2);
  indices.push_back(index4);
  indices.push_back(index3);
}


void BSP::addBlock(int x, int y, int z, int id)
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

void BSP::build(World* curWorld, std::shared_ptr<BSPNode>  curRightChunk,std::shared_ptr<BSPNode>  curLeftChunk,std::shared_ptr<BSPNode>  curTopChunk,
                       std::shared_ptr<BSPNode>  curBottomChunk,std::shared_ptr<BSPNode>  curFrontChunk,std::shared_ptr<BSPNode>  curBackChunk)
{
  vertices.clear();
  indices.clear();

  for(int x = 0; x<CHUNKSIZE;x++)
  {
     for(int y = 0;y<CHUNKSIZE;y++)
     {
       for(int z = 0;z<CHUNKSIZE;z++)
       {
         if(!blockExists(x,y,z)) continue;
         float realX = x/10.0f+CHUNKSIZE*xCoord/10.0f;
         float realY = y/10.0f+CHUNKSIZE*yCoord/10.0f;
         float realZ = z/10.0f+CHUNKSIZE*zCoord/10.0f;

         bool topNeigh = false;
         bool bottomNeigh = false;
         bool leftNeigh = false;
         bool rightNeigh = false;
         bool frontNeigh = false;
         bool backNeigh = false;

         if(x+1 >= CHUNKSIZE)
         {
           if(curRightChunk != NULL)
           {
              if(curRightChunk->blockExists(0,y,z)) rightNeigh = true;
           }
         }
         else if(blockExists(x+1,y,z)) rightNeigh = true;

         if(x-1 < 0)
         {
           if(curLeftChunk != NULL)
           {
            if(curLeftChunk->blockExists(CHUNKSIZE-1,y,z)) leftNeigh = true;
           }
         }
         else if(blockExists(x-1,y,z)) leftNeigh = true;

         if(y+1 >= CHUNKSIZE)
         {
           if(curTopChunk != NULL)
           {
              if(curTopChunk->blockExists(x,0,z)) topNeigh = true;
           }
         }
         else if(blockExists(x,y+1,z)) topNeigh = true;

         if(y-1 < 0)
         {
          if(curBottomChunk != NULL)
          {
            if(curBottomChunk->blockExists(x,CHUNKSIZE-1,z)) bottomNeigh = true;
          }
         }
         else if(blockExists(x,y-1,z)) bottomNeigh = true;

         if(z+1 >= CHUNKSIZE)
         {
           if(curBackChunk != NULL)
           {
             if(curBackChunk->blockExists(x,y,0)) backNeigh = true;
           }
         }
         else if(blockExists(x,y,z+1)) backNeigh = true;

         if(z-1 < 0)
         {
           if(curFrontChunk != NULL)
           {
             if(curFrontChunk->blockExists(x,y,CHUNKSIZE-1)) frontNeigh = true;
           }
         }
         else if(blockExists(x,y,z-1)) frontNeigh = true;


         Block* tempBlock = dictionary[(getBlock(x,y,z))];
         if(tempBlock == NULL) return;
         float x1, y1, x2, y2;

         if(!topNeigh)
         {
           tempBlock->getTop(&x1,&y1,&x2,&y2);

           int index1 = addVertex(realX     , realY+0.1f, realZ     ,0.0f,1.0f,0.0f,x1,y1);
           int index2 = addVertex(realX+0.1f, realY+0.1f, realZ     ,0.0f,1.0f,0.0f,x2,y1);
           int index3 = addVertex(realX     , realY+0.1f, realZ+0.1f,0.0f,1.0f,0.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,0.0f,1.0f,0.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!bottomNeigh)
         {
           tempBlock->getBottom(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY, realZ     ,0.0f,-1.0f,0.0f,x1,y1);
           int index3 = addVertex(realX+0.1f, realY, realZ     ,0.0f,-1.0f,0.0f,x2,y1);
           int index2 = addVertex(realX     , realY, realZ+0.1f,0.0f,-1.0f,0.0f,x1,x2);
           int index4 = addVertex(realX+0.1f, realY, realZ+0.1f,0.0f,-1.0f,0.0f,x2,x2);

           addIndices(index1,index2,index3,index4);
         }

         if(!rightNeigh)
         {
           tempBlock->getRight(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX+0.1f, realY,      realZ     ,1.0f,0.0f,0.0f,x1,y1);
           int index3 = addVertex(realX+0.1f, realY+0.1f, realZ     ,1.0f,0.0f,0.0f,x2,y1);
           int index2 = addVertex(realX+0.1f, realY,      realZ+0.1f,1.0f,0.0f,0.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,1.0f,0.0f,0.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!leftNeigh)
         {
           tempBlock->getLeft(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX, realY,      realZ     ,-1.0f,0.0f,0.0f,x1,y1);
           int index2 = addVertex(realX, realY+0.1f, realZ     ,-1.0f,0.0f,0.0f,x2,y1);
           int index3 = addVertex(realX, realY,      realZ+0.1f,-1.0f,0.0f,0.0f,x1,y2);
           int index4 = addVertex(realX, realY+0.1f, realZ+0.1f,-1.0f,0.0f,0.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }
         if(!backNeigh)
         {
           tempBlock->getBack(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY,      realZ+0.1f,0.0f,0.0f,1.0f,x1,y1);
           int index3 = addVertex(realX+0.1f, realY,      realZ+0.1f,0.0f,0.0f,1.0f,x2,y1);
           int index2 = addVertex(realX     , realY+0.1f, realZ+0.1f,0.0f,0.0f,1.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,0.0f,0.0f,1.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!frontNeigh)
         {
           tempBlock->getFront(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY,      realZ,0.0f,0.0f,-1.0f,x1,y1);
           int index2 = addVertex(realX+0.1f, realY,      realZ,0.0f,0.0f,-1.0f,x2,y1);
           int index3 = addVertex(realX     , realY+0.1f, realZ,0.0f,0.0f,-1.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ,0.0f,0.0f,-1.0f,x2,y2);
           addIndices(index1,index2,index3,index4);
         }
       }
     }
  }
}

void BSP::render()
{
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat),&vertices.front(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint),&indices.front(), GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

}

void BSP::draw(Camera* camera,float lightposx,float lightposy,float lightposz)
{
  glBindTexture(GL_TEXTURE_2D, *glTexture);
  blockShader->Use();

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920/ (float)1080, 0.1f, 100.0f);
  blockShader->setMat4("projection", projection);

  glm::mat4 view = camera->getViewMatrix();
  blockShader->setMat4("view", view);

  blockShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
  blockShader->setVec3("lightColor",  1.0f, 1.0f, 1.0f);
  blockShader->setVec3("lightPos",  lightposx, lightposx, lightposx);
  blockShader->setVec3("viewPos", camera->position);
  glm::mat4 model;
  glm::vec3 newPos;
  newPos.x = 0.0f;
  newPos.y = 0.0f;
  newPos.z = 0.0f;
  model = glm::translate(model, newPos);
  blockShader->setMat4("model", model);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);
  glBindVertexArray(0);

}

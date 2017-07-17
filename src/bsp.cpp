
enum Type {STATIC,DYNAMIC,STREAM};

#include <string>
#include "headers/SOIL.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <unordered_map>
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
#include "headers/block.h"
#include "headers/bsp.h"

BSP::BSP()
{
  std::cout << "rendering bsp shader \n";
  blockShader = new Shader("../src/shaders/shaderBSP.vs","../src/shaders/shaderBSP.fs");
  std::cout << "finished rendering bsp shader \n";
  texture = "../assets/textures/atlas.png";
  totalVertices = 0;

  Block tempBlock;
  tempBlock.id = 1;
  for(int x=0;x<12;x++)
  {
    tempBlock.texArray[x]=0;
  }
  tempBlock.width = 128;
  tempBlock.height = 128;
  tempBlock.atlasWidth = 128*2;
  tempBlock.atlasHeight = 128;
  dictionary.push_back(tempBlock);
  dictionary.push_back(tempBlock);
}

int BSP::addVertex(float x, float y, float z, float texX, float texY)
{
  int numbVert = vertices.size()/5;

  vertices.push_back(x);
  vertices.push_back(y);
  vertices.push_back(z);

  vertices.push_back(texX);
  vertices.push_back(texY);
  return numbVert;
}

bool BSP::existsAt(int x, int y, int z)
{
  if(worldMap.count(x) == 1)
  {
    if(worldMap[x].count(y) == 1)
    {
      if(worldMap[x][y].count(z) == 1)
      {
        return true;
      }
    }
  }
}

void BSP::addIndices(int index1, int index2, int index3, int index4)
{
  indices.push_back(index1);
  indices.push_back(index2);
  indices.push_back(index3);

  indices.push_back(index2);
  indices.push_back(index3);
  indices.push_back(index4);
}


bool BSP::addBlock(int x, int y, int z, int id)
{
  worldMap[x][y][z] = id;
}

int BSP::removeBlock(int x, int y, int z)
{
  worldMap[x][y].erase(z);
}

void BSP::render()
{
  vertices.clear();
  indices.clear();
  typedef std::map< int, int>  z_t;
  typedef std::map< int, z_t > y_t;
  typedef std::map< int, y_t > x_t;
  typedef z_t::iterator        z_iter_t;
  typedef y_t::iterator        y_iter_t;
  typedef x_t::iterator        x_iter_t;

  for(x_iter_t xIndex = worldMap.begin(); xIndex!= worldMap.end();xIndex++)
  {
     for(y_iter_t yIndex = xIndex->second.begin(); yIndex!= xIndex->second.end();yIndex++)
     {
       for(z_iter_t zIndex = yIndex->second.begin(); zIndex != yIndex->second.end();zIndex++)
       {
         int x = xIndex->first;
         int y = yIndex->first;
         int z = zIndex->first;

         float realX = x/10.0f;
         float realY = y/10.0f;
         float realZ = z/10.0f;

         bool topNeigh = false;
         bool bottomNeigh = false;
         bool leftNeigh = false;
         bool rightNeigh = false;
         bool frontNeigh = false;
         bool backNeigh = false;

         if(existsAt(x+1,y,z)) rightNeigh = true;
         if(existsAt(x-1,y,z)) leftNeigh = true;
         if(existsAt(x,y+1,z)) topNeigh = true;
         if(existsAt(x,y-1,z)) bottomNeigh = true;
         if(existsAt(x,y,z+1)) backNeigh = true;
         if(existsAt(x,y,z-1)) frontNeigh = true;


         Block tempBlock = dictionary.at(worldMap[x][y][z]);
         float x1, y1, x2, y2;

         if(!topNeigh)
         {
           tempBlock.getTop(&x1,&y1,&x2,&y2);

           int index1 = addVertex(realX     , realY+0.1f, realZ,     x1,y1);
           int index2 = addVertex(realX+0.1f, realY+0.1f, realZ,     x2,y1);
           int index3 = addVertex(realX     , realY+0.1f, realZ+0.1f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!bottomNeigh)
         {
           tempBlock.getBottom(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY, realZ,     x1,y1);
           int index2 = addVertex(realX+0.1f, realY, realZ,     x2,y1);
           int index3 = addVertex(realX     , realY, realZ+0.1f,x1,x2);
           int index4 = addVertex(realX+0.1f, realY, realZ+0.1f,x2,x2);

           addIndices(index1,index2,index3,index4);
         }

         if(!rightNeigh)
         {
           tempBlock.getRight(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX+0.1f, realY,      realZ,     x1,y1);
           int index2 = addVertex(realX+0.1f, realY+0.1f, realZ,     x2,y1);
           int index3 = addVertex(realX+0.1f, realY,      realZ+0.1f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!leftNeigh)
         {
           tempBlock.getLeft(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX, realY,      realZ,     x1,y1);
           int index2 = addVertex(realX, realY+0.1f, realZ,     x2,y1);
           int index3 = addVertex(realX, realY,      realZ+0.1f,x1,y2);
           int index4 = addVertex(realX, realY+0.1f, realZ+0.1f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }
         if(!backNeigh)
         {
           tempBlock.getBack(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY,      realZ+0.1f,x1,y1);
           int index2 = addVertex(realX+0.1f, realY,      realZ+0.1f,x2,y1);
           int index3 = addVertex(realX     , realY+0.1f, realZ+0.1f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!frontNeigh)
         {
           tempBlock.getFront(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY,      realZ,x1,y1);
           int index2 = addVertex(realX+0.1f, realY,      realZ,x2,y1);
           int index3 = addVertex(realX     , realY+0.1f, realZ,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ,x2,y2);
           addIndices(index1,index2,index3,index4);
         }
       }
     }
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat),&vertices.front(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint),&indices.front(), GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,2,GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenTextures(1, &glTexture);
  glBindTexture(GL_TEXTURE_2D, glTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //Load and bind the texture from the class
  int texWidth, texHeight;
  unsigned char* image = SOIL_load_image(texture, &texWidth, &texHeight, 0 , SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,0,GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  std::cout << vertices.size()/5 << "and index" << indices.size()/3 << "\n";
}

void BSP::draw(glm::mat4 camera)
{
  glBindTexture(GL_TEXTURE_2D, glTexture);
  blockShader->Use();

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800/ (float)600, 0.1f, 100.0f);
  blockShader->setMat4("projection", projection);

  glm::mat4 view = camera;
  blockShader->setMat4("view", view);

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

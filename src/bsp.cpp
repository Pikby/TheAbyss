
enum Type {STATIC,DYNAMIC,STREAM};

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
#include <mutex>
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



World::World()
{

  int seed = 1737;

  perlin = new siv::PerlinNoise(seed);

  blockShader = new Shader("../src/shaders/shaderBSP.vs","../src/shaders/shaderBSP.fs");
  const char* texture = "../assets/textures/atlas.png";
  loadDictionary("../assets/dictionary.dat");

  for(int x=0 ;x<dictionary.size();x++)
  {
    for(int i =0;i<12;i++)
    {
      std::cout << dictionary[x].texArray[i] << ",";
    }
    std::cout <<  dictionary[x].width <<"\n";
    std::cout <<  dictionary[x].height <<"\n";
    std::cout <<  dictionary[x].atlasWidth <<"\n";
    std::cout <<  dictionary[x].atlasHeight <<"\n";
  }

  horzRenderDistance = 5;
  vertRenderDistance = 5;
  totalChunks = 0;
  lightposx = -10.0f;
  lightposy = 10.0f;
  lightposz = 0.0f;

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


}


void World::generateChunk(int chunkx, int chunky, int chunkz)
{
  BSP* testChunk = getChunk(chunkx,chunky,chunkz);
  if(testChunk != NULL)
  {
    return;
  }




  BSPmap[chunkx][chunky][chunkz] = BSP(blockShader,&dictionary,&glTexture,chunkx,chunky,chunkz,perlin);
  BSP* tempChunk = &BSPmap[chunkx][chunky][chunkz];
  BSP* otherChunk;


  otherChunk = getChunk(chunkx,chunky,chunkz-1);
  if(otherChunk  != NULL )
  {
    tempChunk->frontChunk = otherChunk;
    otherChunk->backChunk = tempChunk;
    if(!otherChunk->toRender) buildQueue.push(otherChunk);
  }

  otherChunk = getChunk(chunkx,chunky,chunkz+1);
  if(otherChunk != NULL)
  {
    tempChunk->backChunk = otherChunk;
    otherChunk->frontChunk = tempChunk;
    if(!otherChunk->toRender) buildQueue.push(otherChunk);
  }

  otherChunk = getChunk(chunkx,chunky-1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->bottomChunk = otherChunk;
    otherChunk->topChunk = tempChunk;
    if(!otherChunk->toRender) buildQueue.push(otherChunk);
  }

  otherChunk = getChunk(chunkx,chunky+1,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->topChunk = otherChunk;
    otherChunk->bottomChunk = tempChunk;
    if(!otherChunk->toRender) buildQueue.push(otherChunk);
  }

  otherChunk = getChunk(chunkx-1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->leftChunk = otherChunk;
    otherChunk->rightChunk = tempChunk;
    if(!otherChunk->toRender) buildQueue.push(otherChunk);
  }

  otherChunk = getChunk(chunkx+1,chunky,chunkz);
  if(otherChunk != NULL)
  {
    tempChunk->rightChunk = otherChunk;
    otherChunk->leftChunk = tempChunk;
    if(!otherChunk->toRender) buildQueue.push(otherChunk);
  }
    if(!tempChunk->toRender) buildQueue.push(tempChunk);
}

void World::renderWorld(float* mainx, float* mainy, float* mainz)
{
  for(int curDis = 0; curDis < horzRenderDistance; curDis++)
  {
    for(int height = 0; height < vertRenderDistance*2; height++)
    {
      int x = round(*mainx);
      int y = round(*mainy);
      int z = round(*mainz);
      x/= CHUNKSIZE;
      y/= CHUNKSIZE;
      z/= CHUNKSIZE;
      y--;

      int ychunk;
      if(height % 2 == 1) ychunk = y -(height/2);
      else ychunk = y + (height/2);

      int xchunk = x;
      int zchunk = z;


      zchunk = z+curDis;
      xchunk = x;
      for(int i = -curDis+1;i<curDis;i++)
      {
        generateChunk(xchunk+i,ychunk,zchunk);
      }

      zchunk = z-curDis;
      xchunk = x;
      for(int i = -curDis+1;i<curDis;i++)
      {
        generateChunk(xchunk+i,ychunk,zchunk);
      }
      zchunk = z;
      xchunk = x+curDis;
      for(int i = -curDis;i<=curDis;i++)
      {
        generateChunk(xchunk,ychunk,zchunk+i);
      }
      zchunk = z;
      xchunk = x-curDis;
      for(int i = -curDis;i<=curDis;i++)
      {
        generateChunk(xchunk,ychunk,zchunk+i);
      }
    }
  }
}

BSP* World::getChunk(int x, int y, int z)
{
  if(BSPmap.count(x) == 1)
  {
    if(BSPmap[x].count(y) == 1)
    {
      if(BSPmap[x][y].count(z) == 1)
      {
        return &BSPmap[x][y][z];
      }
    }
  }
  return NULL;
}

void World::delChunk(int x, int y, int z)
{
  std::cout << "Deleting chunk: " << x << ":" << y << ":" << z << "\n";
  BSPmap[x][y].erase(z);
  if(BSPmap[x][y].empty())
  {
    BSPmap[x].erase(y);
    if(BSPmap[x].empty())
    {
      BSPmap.erase(x);
    }
  }
}

void World::drawWorld(Camera* camera)
{
  lightposx += 0.01;
  typedef std::unordered_map <int, BSP> z_t;
  typedef std::unordered_map <int, z_t> y_t;
  typedef std::unordered_map <int, y_t> x_t;
  typedef x_t::iterator x_iter_t;
  typedef y_t::iterator y_iter_t;
  typedef z_t::iterator z_iter_t;

  for(x_iter_t xi = BSPmap.begin(); xi != BSPmap.end(); xi++)
  {
    for(y_iter_t yi = xi->second.begin(); yi != xi->second.end(); yi++)
    {
      for(z_iter_t zi = yi->second.begin(); zi != yi->second.end(); zi++)
      {
        if(zi->second.toDelete) zi++;
        zi->second.draw(camera,lightposx,lightposy,lightposz);
      }
    }
  }

  /*
  while(!delQueue.empty())
  {
    std::cout << "Deleting chunk \n";
    BSP* tempChunk = delQueue.front();
    delQueue.pop();
    delChunk(tempChunk->xCoord,tempChunk->yCoord,tempChunk->zCoord);
  }
  */

}

void World::delScan(int x, int y, int z)
{
  x/= CHUNKSIZE;
  y/= CHUNKSIZE;
  z/= CHUNKSIZE;
  typedef std::unordered_map <int, BSP> z_t;
  typedef std::unordered_map <int, z_t> y_t;
  typedef std::unordered_map <int, y_t> x_t;
  typedef x_t::iterator x_iter_t;
  typedef y_t::iterator y_iter_t;
  typedef z_t::iterator z_iter_t;

  while(delQueue.empty())
  {
    //Wait till the queue empties
  }

  for(x_iter_t xi = BSPmap.begin(); xi != BSPmap.end(); xi++)
  {
    for(y_iter_t yi = xi->second.begin(); yi != xi->second.end(); yi++)
    {
      for(z_iter_t zi = yi->second.begin(); zi != yi->second.end(); zi++)
      {
        if(zi->second.toDelete = true) continue;
        int chunkx = xi->first;
        int chunky = yi->first;
        int chunkz = zi->first;

        if(abs(abs(chunkx) - abs(x)) > horzRenderDistance
        || abs(abs(chunky) - abs(y)) > horzRenderDistance
        || abs(abs(chunkz) - abs(z)) > horzRenderDistance)
        {
          if(zi->second.toDelete != true)
          {
            zi->second.toDelete = true;
            delQueue.push(&zi->second);

          }
        }
      }
    }
  }





}

bool World::chunkExists(int x ,int y, int z)
{
  if(BSPmap.count(x) == 1)
  {
    if(BSPmap[x].count(y) == 1)
    {
      if(BSPmap[x][y].count(z) == 1)
      {
        return true;
      }
    }
  }
  return false;
}

bool World::blockExists(int x, int y, int z)
{
  int xlocal = x >= 0 ? x % CHUNKSIZE : CHUNKSIZE + (x % CHUNKSIZE);
  int ylocal = y >= 0 ? y % CHUNKSIZE : CHUNKSIZE + (y % CHUNKSIZE);
  int zlocal = z >= 0 ? z % CHUNKSIZE : CHUNKSIZE + (z % CHUNKSIZE);

  if(xlocal == CHUNKSIZE)xlocal = 0;
  if(ylocal == CHUNKSIZE)ylocal = 0;
  if(zlocal == CHUNKSIZE)zlocal = 0;

  int xchunk = floor((float)x/(float)CHUNKSIZE);
  int ychunk = floor((float)y/(float)CHUNKSIZE);
  int zchunk = floor((float)z/(float)CHUNKSIZE);

  BSP* tempChunk;
  if(tempChunk = getChunk(xchunk,ychunk,zchunk))
  {

    if(tempChunk->blockExists(xlocal,ylocal, zlocal))
    {
      return true;
    }
  }
  return false;
}

BSP::BSP(Shader* shader, std::vector<Block> * dict, GLuint* newglTexture, long int x, long int y, long int z,  siv::PerlinNoise* perlin)
{

  xCoord = x;
  yCoord = y;
  zCoord = z;
  toRender = false;
  isBuilt = false;
  isDrawing = false;
  toDelete = false;


  blockShader = shader;
  dictionary = dict;
  glTexture = newglTexture;

  leftChunk = NULL;
  rightChunk = NULL;
  frontChunk = NULL;
  backChunk = NULL;
  topChunk = NULL;
  bottomChunk = NULL;

  for(int x = 0;x<CHUNKSIZE*CHUNKSIZE*CHUNKSIZE;x++)
    worldMap[x] = 0;

  generateTerrain(perlin);
}

BSP::~BSP()
{

  if(leftChunk != NULL) leftChunk->rightChunk = NULL;
  if(rightChunk != NULL) rightChunk->leftChunk = NULL;
  if(frontChunk != NULL) frontChunk->backChunk = NULL;
  if(backChunk != NULL) backChunk->frontChunk = NULL;
  if(topChunk != NULL) topChunk->bottomChunk = NULL;
  if(bottomChunk != NULL) bottomChunk->topChunk = NULL;

}

BSP::BSP(){}

Block::Block(int newId, int* array, int newWidth,int newHeight,int newAtlasWidth, int newAtlasHeight)
{
  id = newId;
  for(int x=0;x<12;x++)
  {
    texArray[x] = array[x];
  }
  width = newWidth;
  height = newHeight;
  atlasWidth = newAtlasWidth;
  atlasHeight = newAtlasHeight;
}

bool World::loadDictionary(const char* file)
{
  int id = 0;
  using namespace std;
  string line;
  ifstream dictionaryf (file);
  if(dictionaryf.is_open())
  {
    getline(dictionaryf,line);
    int atlasWidth = stoi(line);
    getline(dictionaryf,line);
    int atlasHeight = stoi(line);

    while(getline(dictionaryf,line))
    {
      int texNumb;
      int texArray[12];
      stringstream ss;
      ss << line;
      for(int x = 0;x<12;x++)
      {
        ss >> texNumb;
        texArray[x] = texNumb;
        if(ss.peek() == ',') ss.ignore();
      }
      getline(dictionaryf,line);
      int width = stoi(line);
      getline(dictionaryf,line);
      int height = stoi(line);

      dictionary.push_back(Block(id++,texArray,width,height,atlasWidth,atlasHeight) );
    }
  }
}

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

void BSP::build(World* curWorld)
{


  while(isDrawing||toRender)
  {
    //Wait till it stops drawing
  }
  isBuilt = false;
  vertices.clear();
  indices.clear();
  BSP* curRightChunk = rightChunk;
  BSP* curLeftChunk = leftChunk;
  BSP* curTopChunk = topChunk;
  BSP* curBottomChunk = bottomChunk;
  BSP* curFrontChunk = frontChunk;
  BSP* curBackChunk = backChunk;



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
              if(curRightChunk->blockExists(0,y,z)) rightNeigh = true;
         }
         else if(blockExists(x+1,y,z)) rightNeigh = true;

         if(x-1 < 0)
         {
          if(curLeftChunk != NULL)
            if(curLeftChunk->blockExists(CHUNKSIZE-1,y,z)) leftNeigh = true;
         }
         else if(blockExists(x-1,y,z)) leftNeigh = true;

         if(y+1 >= CHUNKSIZE)
         {
           if(curTopChunk != NULL)
              if(curTopChunk->blockExists(x,0,z)) topNeigh = true;
         }
         else if(blockExists(x,y+1,z)) topNeigh = true;

         if(y-1 < 0)
         {
          if(curBottomChunk != NULL)
            if(curBottomChunk->blockExists(x,CHUNKSIZE-1,z)) bottomNeigh = true;
         }
         else if(blockExists(x,y-1,z)) bottomNeigh = true;

         if(z+1 >= CHUNKSIZE)
         {
           if(curBackChunk != NULL)
             if(curBackChunk->blockExists(x,y,0)) backNeigh = true;
         }
         else if(blockExists(x,y,z+1)) backNeigh = true;

         if(z-1 < 0)
         {
           if(curFrontChunk != NULL)
             if(curFrontChunk->blockExists(x,y,CHUNKSIZE-1)) frontNeigh = true;
         }
         else if(blockExists(x,y,z-1)) frontNeigh = true;


         Block* tempBlock = &dictionary->at(getBlock(x,y,z));

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
  isBuilt = true;
  toRender = true;
}

void BSP::render()
{

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

  toRender = false;
}

void BSP::draw(Camera* camera,float lightposx,float lightposy,float lightposz)
{


  if(!isBuilt || toDelete)
  {
    return;
  }
  if(toRender) render();

  isDrawing = true;
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

  isDrawing = false;
}

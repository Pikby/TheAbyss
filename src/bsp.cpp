
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

  horzRenderDistance = 15;
  vertRenderDistance = 15;
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

void World::renderWorld(int x, int y, int z)
{

  for(int i = 0; i < horzRenderDistance;i++)
  {
    for(int k = 0; k <  horzRenderDistance;k++)
    {
      for(int j = 0; j < vertRenderDistance;j++)
      {
        for(int a = 0; a < 8;a++)
        {
          int chunkx = x+i;
          int chunky = y+j;
          int chunkz = z+k;
          switch(a)
          {
            case(1): chunkx = x-i; break;
            case(2): chunky = y-j; break;
            case(3): chunkz = z-k; break;
            case(4): chunkx = x-i; chunky = y-j; break;
            case(5): chunkx = x-i; chunkz = z-k; break;
            case(6): chunky = y-j; chunkz = z-k; break;
            case(7): chunkx = z-i; chunky = y-j; chunkz = z-k; break;
          }

          //std::cout << "checking for chunk:   " << chunkx << ":" << chunky << ":" << chunkz << "\n";
          if(!chunkExists(chunkx,chunky,chunkz))
          {
            //std::cout << "creating  chunk:   " << chunkx << ":" << chunky << ":" << chunkz << "\n";

            BSPmap[chunkx][chunky][chunkz] = BSP(blockShader,&dictionary,&glTexture,chunkx,chunky,chunkz,perlin);
            BSP* tempChunk = &BSPmap[chunkx][chunky][chunkz];
            BSP* otherChunk;

            otherChunk = getChunk(chunkx,chunky,chunkz-1);
            if(otherChunk  != NULL )
            {
              tempChunk->frontChunk = otherChunk;
              otherChunk->backChunk = tempChunk;
              if(otherChunk->isBuilt)
              {
                otherChunk->build(this);
                renderQueue.push(otherChunk);
              }
            }

            otherChunk = getChunk(chunkx,chunky,chunkz+1);
            if(otherChunk != NULL)
            {
              tempChunk->backChunk = otherChunk;
              otherChunk->frontChunk = tempChunk;
              if(otherChunk->isBuilt)
              {
                otherChunk->build(this);
                renderQueue.push(otherChunk);
              }
            }

            otherChunk = getChunk(chunkx,chunky-1,chunkz);
            if(otherChunk != NULL)
            {
              tempChunk->bottomChunk = otherChunk;
              otherChunk->topChunk = tempChunk;
              if(otherChunk->isBuilt)
              {
                otherChunk->build(this);
                renderQueue.push(otherChunk);
              }
            }

            otherChunk = getChunk(chunkx,chunky+1,chunkz);
            if(otherChunk != NULL)
            {
              tempChunk->topChunk = otherChunk;
              otherChunk->bottomChunk = tempChunk;
              if(otherChunk->isBuilt)
              {
                otherChunk->build(this);
                renderQueue.push(otherChunk);
              }
            }

            otherChunk = getChunk(chunkx-1,chunky,chunkz);
            if(otherChunk != NULL)
            {
              tempChunk->leftChunk = otherChunk;
              otherChunk->rightChunk = tempChunk;
              if(otherChunk->isBuilt)
              {
                otherChunk->build(this);
                renderQueue.push(otherChunk);
              }
            }

            otherChunk = getChunk(chunkx+1,chunky,chunkz);
            if(otherChunk != NULL)
            {
              tempChunk->rightChunk = otherChunk;
              otherChunk->leftChunk = tempChunk;
              if(otherChunk->isBuilt)
              {
                otherChunk->build(this);
                renderQueue.push(otherChunk);
              }
            }

            tempChunk->build(this);
            renderQueue.push(tempChunk);
          }
        }
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

void World::drawWorld(int x, int y, int z, Camera* camera)
{
  lightposx += 0.01;
  typedef std::unordered_map <int, BSP> z_t;
  typedef std::unordered_map <int, z_t> y_t;
  typedef std::unordered_map <int, y_t> x_t;
  typedef x_t::iterator x_iter_t;
  typedef y_t::iterator y_iter_t;
  typedef z_t::iterator z_iter_t;

  std::vector<BSP*> toDelete;


  for(x_iter_t xi = BSPmap.begin(); xi != BSPmap.end(); xi++)
  {
    for(y_iter_t yi = xi->second.begin(); yi != xi->second.end(); yi++)
    {
      for(z_iter_t zi = yi->second.begin(); zi != yi->second.end(); zi++)
      {
        int i = xi->first;
        int j = yi->first;
        int k = zi->first;

        //std::cout << i << ":" << j << ":" << k << "\n";
        if(sqrt(pow(x-i,2)+pow(z-k,2)) > horzRenderDistance+2 || abs(j-y) > vertRenderDistance +2)
        {
          //toDelete.push_back(&zi->second);
        }
        else if(zi->second.isRendered && zi->second.isBuilt) zi->second.draw(camera,lightposx,lightposy,lightposz);
      }
    }
  }
  for(int x=0;x<toDelete.size();x++)
  {
    if(toDelete[x]->isBuilt && toDelete[x]->isRendered);
      //delChunk(toDelete[x]->xCoord,toDelete[x]->yCoord,toDelete[x]->zCoord);
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
  //std::cout << "checking for block" << x << ":" << y << ":" << z << "\n";
  int xlocal = x >= 0 ? x % 16 : 16 + (x % 16);
  int ylocal = y >= 0 ? y % 16 : 16 + (y % 16);
  int zlocal = z >= 0 ? z % 16 : 16 + (z % 16);

  if(xlocal == 16)xlocal = 0;
  if(ylocal == 16)ylocal = 0;
  if(zlocal == 16)zlocal = 0;

  int xchunk = floor((float)x/(float)16);
  int ychunk = floor((float)y/(float)16);
  int zchunk = floor((float)z/(float)16);

  //std::cout << "checking for block in chunk: " << xchunk <<":" << zchunk << "withCoords: " << xlocal << ":" <<y << ":" << zlocal << "\n";
  if(chunkExists(xchunk,ychunk,zchunk))
  {
    //std::cout << BSPmap[xchunk][zchunk].xCoord << BSPmap[xchunk][zchunk].zCoord << "\n";
    if(BSPmap[xchunk][ychunk][zchunk].blockExists(xlocal,ylocal, zlocal))
    {
      //std::cout << "found block ahead\n";
      return true;
    }
  }
      //std::cout << "no block ahead\n";
  return false;
}

BSP::BSP(Shader* shader, std::vector<Block> * dict, GLuint* newglTexture, long int x, long int y, long int z,  siv::PerlinNoise* perlin)
{
  isRendered = false;
  isDrawing = false;
  isBuilt = false;
  xCoord = x;
  yCoord = y;
  zCoord = z;

  for(int x = 0;x<16*16*16;x++)
    worldMap[x] = 0;

  generateTerrain(perlin);

  blockShader = shader;
  dictionary = dict;
  glTexture = newglTexture;

  leftChunk = NULL;
  rightChunk = NULL;
  frontChunk = NULL;
  backChunk = NULL;
  topChunk = NULL;
  bottomChunk = NULL;
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


  for(int x=0;x<16;x++)
  {
    for(int y=0;y<16;y++)
    {
      for(int z=0;z<16;z++)
      {
        int height = 16+200*perlin->octaveNoise0_1((x+xCoord*16)/freq,(z+zCoord*16)/freq,oct);
        if(yCoord*16+y <height)
        {
          if(yCoord*16+y == height - 1) addBlock(x,y,z,2);
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
  if(x>=16 || y >=16 || z >= 16 || x<0 || y<0 || z<0) return false ;
  if(worldMap[x+16*y+z*16*16] == 0) return false;
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


bool BSP::addBlock(int x, int y, int z, int id)
{
  worldMap[x+y*16+z*16*16] = id;
}

int BSP::removeBlock(int x, int y, int z)
{
  worldMap[x + y*16 + z*16*16] = 0;
}

int BSP::getBlock(int x, int y, int z)
{
  return worldMap[x+y*16+z*16*16];
}

void BSP::build(World* curWorld)
{
  isBuilt = false;
  while(isDrawing)
  {
    //Wait till the current rendering is done
  }

  vertices.clear();
  indices.clear();

  for(int x = 0; x<16;x++)
  {
     for(int y = 0;y<16;y++)
     {
       for(int z = 0;z<16;z++)
       {
         if(!blockExists(x,y,z)) continue;
         float realX = x/10.0f+16*xCoord/10.0f;
         float realY = y/10.0f+16*yCoord/10.0f;
         float realZ = z/10.0f+16*zCoord/10.0f;

         bool topNeigh = false;
         bool bottomNeigh = false;
         bool leftNeigh = false;
         bool rightNeigh = false;
         bool frontNeigh = false;
         bool backNeigh = false;

         if(x+1 >= 16)
         {
           if(rightChunk != NULL)
              if(rightChunk->blockExists(0,y,z)) rightNeigh = true;
         }
         else if(blockExists(x+1,y,z)) rightNeigh = true;

         if(x-1 < 0)
         {
          if(leftChunk != NULL)
            if(leftChunk->blockExists(15,y,z)) leftNeigh = true;
         }
         else if(blockExists(x-1,y,z)) leftNeigh = true;

         if(y+1 >= 16)
         {
           if(topChunk != NULL)
              if(topChunk->blockExists(x,0,z)) topNeigh = true;
         }
         else if(blockExists(x,y+1,z)) topNeigh = true;

         if(y-1 < 0)
         {
          if(bottomChunk != NULL)
            if(bottomChunk->blockExists(x,15,z)) bottomNeigh = true;
         }
         else if(blockExists(x,y-1,z)) bottomNeigh = true;

         if(z+1 >= 16)
         {
           if(backChunk != NULL)
             if(backChunk->blockExists(x,y,0)) backNeigh = true;
         }
         else if(blockExists(x,y,z+1)) backNeigh = true;

         if(z-1 < 0)
         {
           if(frontChunk != NULL)
             if(frontChunk->blockExists(x,y,15)) frontNeigh = true;
         }
         else if(blockExists(x,y,z-1)) frontNeigh = true;


         Block tempBlock = dictionary->at(getBlock(x,y,z));

         float x1, y1, x2, y2;

         if(!topNeigh)
         {
           tempBlock.getTop(&x1,&y1,&x2,&y2);

           int index1 = addVertex(realX     , realY+0.1f, realZ     ,0.0f,1.0f,0.0f,x1,y1);
           int index2 = addVertex(realX+0.1f, realY+0.1f, realZ     ,0.0f,1.0f,0.0f,x2,y1);
           int index3 = addVertex(realX     , realY+0.1f, realZ+0.1f,0.0f,1.0f,0.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,0.0f,1.0f,0.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!bottomNeigh)
         {
           tempBlock.getBottom(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY, realZ     ,0.0f,-1.0f,0.0f,x1,y1);
           int index3 = addVertex(realX+0.1f, realY, realZ     ,0.0f,-1.0f,0.0f,x2,y1);
           int index2 = addVertex(realX     , realY, realZ+0.1f,0.0f,-1.0f,0.0f,x1,x2);
           int index4 = addVertex(realX+0.1f, realY, realZ+0.1f,0.0f,-1.0f,0.0f,x2,x2);

           addIndices(index1,index2,index3,index4);
         }

         if(!rightNeigh)
         {
           tempBlock.getRight(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX+0.1f, realY,      realZ     ,1.0f,0.0f,0.0f,x1,y1);
           int index3 = addVertex(realX+0.1f, realY+0.1f, realZ     ,1.0f,0.0f,0.0f,x2,y1);
           int index2 = addVertex(realX+0.1f, realY,      realZ+0.1f,1.0f,0.0f,0.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,1.0f,0.0f,0.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!leftNeigh)
         {
           tempBlock.getLeft(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX, realY,      realZ     ,-1.0f,0.0f,0.0f,x1,y1);
           int index2 = addVertex(realX, realY+0.1f, realZ     ,-1.0f,0.0f,0.0f,x2,y1);
           int index3 = addVertex(realX, realY,      realZ+0.1f,-1.0f,0.0f,0.0f,x1,y2);
           int index4 = addVertex(realX, realY+0.1f, realZ+0.1f,-1.0f,0.0f,0.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }
         if(!backNeigh)
         {
           tempBlock.getBack(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY,      realZ+0.1f,0.0f,0.0f,1.0f,x1,y1);
           int index3 = addVertex(realX+0.1f, realY,      realZ+0.1f,0.0f,0.0f,1.0f,x2,y1);
           int index2 = addVertex(realX     , realY+0.1f, realZ+0.1f,0.0f,0.0f,1.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,0.0f,0.0f,1.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!frontNeigh)
         {
           tempBlock.getFront(&x1,&y1,&x2,&y2);
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
  //std::cout << "vertices added rendering chunk\n";
}

void BSP::render()
{
  isRendered = false;
  while(isDrawing || !isBuilt)
  {
    //Wait till the chunk stops drawing
  }


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

  isRendered = true;
}

void BSP::draw(Camera* camera,float lightposx,float lightposy,float lightposz)
{
  //std::cout << "drawing chunk" << xCoord << ":" << yCoord << ":" << zCoord << "\n";
  isDrawing = true;
  while(!isRendered || !isBuilt)
  {
    //Wait till the chunk stops being built or rendered
  }

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

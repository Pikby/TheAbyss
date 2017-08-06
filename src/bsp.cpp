
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
#include "headers/perlinnoise.h"


World::World()
{

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

  renderDistance = 5;
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

void World::renderWorld(int x, int z)
{
  for(int i = x - renderDistance; i < x + renderDistance;i++)
  {
    for(int j = z - renderDistance; j < z + renderDistance;j++)
    {
      if(!chunkExists(i,j))
      {
        BSPmap[i][j] = BSP(blockShader,&dictionary,&glTexture,i,j);

        BSPmap[i][j].render(this);
      }
    }
  }
}



void World::delChunk(int x, int z)
{
  BSPmap[x].erase(z);
  if(BSPmap[x].empty())
  {
    BSPmap.erase(x);
  }
}

void World::drawWorld(int x, int z, Camera* camera)
{
  lightposx += 0.01;
  typedef std::map <int, BSP> z_t;
  typedef std::map <int, z_t> x_t;
  typedef x_t::iterator x_iter_t;
  typedef z_t::iterator z_iter_t;

  for(x_iter_t xi = BSPmap.begin(); xi != BSPmap.end(); xi++)
  {
    for(z_iter_t zi = xi->second.begin(); zi != xi->second.end(); zi++)
    {
      int i = xi->first;
      int j = zi->first;

      //std::cout << ":" <<i << ":" << j << "\n";
      if(sqrt(pow(x-i,2)+pow(z-j,2)) >renderDistance+5)
      {

        if(zi == xi->second.begin())
        {
          delChunk(i,j);
          zi = xi->second.begin();
          if(zi == xi->second.end()) break;
        }
        else
        {
          zi--;
          delChunk(i,j);
        }
      }
      else zi->second.draw(camera,lightposx,lightposy,lightposz);
    }

  }
}


bool World::chunkExists(int x , int z)
{
  if(BSPmap.count(x) == 1)
  {
    if(BSPmap[x].count(z) == 1)
    {
      return true;
    }
  }
  return false;
}

bool World::blockExists(int x, int y, int z)
{
  std::cout << "checking for block" << x << ":" << y << ":" << z << "\n";
  int xlocal = x >= 0 ? x % 16 : 16 + (x % 16);
  int zlocal = z >= 0 ? z % 16 : 16 + (z % 16);

  if(xlocal == 16)xlocal = 0;
  if(zlocal == 16)zlocal = 0;
  int xchunk = floor((float)x/(float)16);
  int zchunk = floor((float)z/(float)16);

  std::cout << "checking for block in chunk: " << xchunk <<":" << zchunk << "withCoords: " << xlocal << ":" <<y << ":" << zlocal << "\n";

  if(chunkExists(xchunk,zchunk))
  {
    if(BSPmap[xchunk][zchunk].blockExists(xlocal,y, zlocal))
    {
      std::cout << "found block ahead\n";
      return true;
    }
  }
      std::cout << "no block ahead\n";
  return false;
}

BSP::BSP(Shader* shader, std::vector<Block> * dict, GLuint* newglTexture, long int x, long int z)
{

  for(int x = 0;x<16*256*16;x++)
    worldMap[x] = 0;

  generateTerrain();

  blockShader = shader;
  dictionary = dict;
  xCoord = x;
  zCoord = z;
  glTexture = newglTexture;
}

BSP::BSP()
{

}

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

void BSP::generateTerrain()
{
  double freq = 64;
  int oct = 8;
  int seed = 1337;

  siv::PerlinNoise perlin(seed);

  for(int x=0;x<16;x++)
  {
    for(int z=0;z<16;z++)
    {
        int height = 60*perlin.octaveNoise0_1((x+xCoord*16)/freq,(z+zCoord*16)/freq,oct);
        for(int y = 0; y<height; y++)
        {
          if(y == height - 1) addBlock(x,y,z,2);
          else addBlock(x,y,z,1);
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
  if(x>=16 || y >=256 || z >= 16 || x<0 || y<0 || z<0) return false;
  if(worldMap[x+16*y+z*16*256] == 0) return false;
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
  worldMap[x+y*16+z*16*256] = id;
}

int BSP::removeBlock(int x, int y, int z)
{
  worldMap[x + y*16 + z*16*256] = 0;
}

int BSP::getBlock(int x, int y, int z)
{
  return worldMap[x+y*16+z*16*256];
}

void BSP::render(World* curWorld)
{
  vertices.clear();
  indices.clear();

  for(int x = 0; x<16;x++)
  {
     for(int y = 0;y<256;y++)
     {
       for(int z = 0;z<16;z++)
       {
         if(!blockExists(x,y,z)) continue;
         float realX = x/10.0f+16*xCoord/10.0f;
         float realY = y/10.0f;
         float realZ = z/10.0f+16*zCoord/10.0f;

         bool topNeigh = false;
         bool bottomNeigh = false;
         bool leftNeigh = false;
         bool rightNeigh = false;
         bool frontNeigh = false;
         bool backNeigh = false;

         if(blockExists(x+1,y,z)) rightNeigh = true;
         if(blockExists(x-1,y,z)) leftNeigh = true;
         if(blockExists(x,y+1,z)) topNeigh = true;
         if(blockExists(x,y-1,z)) bottomNeigh = true;
         if(blockExists(x,y,z+1)) backNeigh = true;
         if(blockExists(x,y,z-1)) frontNeigh = true;


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
           int index2 = addVertex(realX+0.1f, realY+0.1f, realZ     ,1.0f,0.0f,0.0f,x2,y1);
           int index3 = addVertex(realX+0.1f, realY,      realZ+0.1f,1.0f,0.0f,0.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,1.0f,0.0f,0.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!leftNeigh)
         {
           tempBlock.getLeft(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX, realY,      realZ     ,-1.0f,0.0f,0.0f,x1,y1);
           int index3 = addVertex(realX, realY+0.1f, realZ     ,-1.0f,0.0f,0.0f,x2,y1);
           int index2 = addVertex(realX, realY,      realZ+0.1f,-1.0f,0.0f,0.0f,x1,y2);
           int index4 = addVertex(realX, realY+0.1f, realZ+0.1f,-1.0f,0.0f,0.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }
         if(!backNeigh)
         {
           tempBlock.getBack(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY,      realZ+0.1f,0.0f,0.0f,1.0f,x1,y1);
           int index2 = addVertex(realX+0.1f, realY,      realZ+0.1f,0.0f,0.0f,1.0f,x2,y1);
           int index3 = addVertex(realX     , realY+0.1f, realZ+0.1f,0.0f,0.0f,1.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ+0.1f,0.0f,0.0f,1.0f,x2,y2);

           addIndices(index1,index2,index3,index4);
         }

         if(!frontNeigh)
         {
           tempBlock.getFront(&x1,&y1,&x2,&y2);
           int index1 = addVertex(realX     , realY,      realZ,0.0f,0.0f,-1.0f,x1,y1);
           int index3 = addVertex(realX+0.1f, realY,      realZ,0.0f,0.0f,-1.0f,x2,y1);
           int index2 = addVertex(realX     , realY+0.1f, realZ,0.0f,0.0f,-1.0f,x1,y2);
           int index4 = addVertex(realX+0.1f, realY+0.1f, realZ,0.0f,0.0f,-1.0f,x2,y2);
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

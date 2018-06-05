#pragma once
#include <string>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
enum Type {NONE,MELEEWEAPON,RANGEDWEAPON};

class Item
{
  std::string name;
  int type;
  int reach = 3;
  int damage = 0.5;

  int mouse1(glm::vec3 pos, glm::vec3 front);
  int rightClick();
};

#define BLOCKRES 128

class Block
{
public:
  std::string name;
  int id;
  int texArray[12]; //array of coordinates of all sides of the block from the texture array
  int width;
  int height;
  int depth;
  int atlasWidth;
  int atlasHeight;
  int visibleType;

  Block(std::string newName, int newId, int* array,int newVisibleType, int newWidth,
  int newHeight, int newDepth,int newAtlasWidth, int newAtlasHeight)
  {
    visibleType = newVisibleType;
    name = newName;
    id = newId;
    for(int x=0;x<12;x++)
    {
      texArray[x] = array[x];
    }
    width = newWidth;
    height = newHeight;
    depth = newDepth;
    atlasWidth = newAtlasWidth;
    atlasHeight = newAtlasHeight;
  }
  Block(){};

  void print()
  {
    std::cout << name << "\n"
              << id << "\n"
              << visibleType << "\n";
  }

  void getTop(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[0]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[1]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[0]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[1]+1);
  };

  void getBottom(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[2]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[3]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[2]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[3]+1);
  };

  void getLeft(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[4]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[5]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[4]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[5]+1);
  };

  void getRight(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[6]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[7]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[6]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[7]+1);
  };

  void getFront(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[8]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[9]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[8]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[9]+1);
  };

  void getBack(float* x1, float* y1, float* x2, float* y2)
  {
    *x1 = ((float)width/(float)atlasWidth)*(float)(texArray[10]);
    *y1 = ((float)height/(float)atlasHeight)*(float)(texArray[11]);
    *x2 = ((float)width/(float)atlasWidth)*(float)(texArray[10]+1);
    *y2 = ((float)height/(float)atlasHeight)*(float)(texArray[11]+1);
  };

  bool isInBlock(glm::vec3 pos)
  {
    if(pos.x>0 && pos.x<(float)width/BLOCKRES)
      if(pos.y>0 && pos.y<(float)height/BLOCKRES)
        if(pos.z>0 && pos.z<(float)depth/BLOCKRES)
        {
          return true;
        }

    return false;
  };

};
class ItemDatabase
{
public:
  static Block* blockDictionary;
  static Item* itemDictionary;
  static bool loadBlockDictionary(const char* file);
  static bool loadItemDictionary(const char* file);
};

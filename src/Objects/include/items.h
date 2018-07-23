#pragma once
enum Type {NONE,MELEEWEAPON,RANGEDWEAPON};
#include <string>
#include <glm/glm.hpp>
typedef unsigned char uchar;

class Block;

class Item
{
  std::string name;
  int type;
  int reach = 3;
  int damage = 0.5;

  int mouse1(glm::vec3 pos, glm::vec3 front);
  int rightClick();
};

class ItemDatabase
{
public:
  static Block* blockDictionary;
  static Item* itemDictionary;
  static bool loadBlockDictionary(const char* file);
  static bool loadItemDictionary(const char* file);
};

#define BLOCKRES 128

class Block
{
public:
  std::string name;
  int id;
  uchar texArray[6];
  int visibleType;

  Block(std::string newName, int newId, int* array,int newVisibleType)
  {
    visibleType = newVisibleType;
    name = newName;
    id = newId;
    for(int x=0;x<6;x++)
    {
      texArray[x] = array[x];
    }
  }
  Block(){};

  void print()
  {
    std::cout << name << "\n"
              << id << "\n"
              << visibleType << "\n";
  }

  uchar getTop()
  {
    return texArray[0];
  };

  uchar getBottom()
  {
    return texArray[1];
  };

  uchar getLeft()
  {
    return texArray[2];
  };

  uchar getRight()
  {
    return texArray[3];
  };

  uchar getFront()
  {
    return texArray[4];
  };

  uchar getBack()
  {
    return texArray[5];
  };


};

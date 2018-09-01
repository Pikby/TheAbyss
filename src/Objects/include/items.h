#pragma once
enum Type {NONE,MELEEWEAPON,RANGEDWEAPON};
enum RenderType{OPAQUE = 0, TRANSLUCENT = 1, TRANSPARENT = 2};
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
typedef unsigned char uchar;

class Block;


class Item
{
public:
  std::string name;
  int id;
  int reach = 3;
  int damage = 0.5;
  virtual void attack1()
  {
    std::cout << "default attack\n";
  };
  virtual void attack2(){};
  virtual void special1(){};
};
enum ItemType {LIGHTSOURCE,FOOD,PLACEABLE};

class LightSourceItem : public Item
{
  virtual void attack1()
  {
    std::cout << "LightSource attack\n";
  }
  virtual void attack2()
  {

  }
  virtual void special1()
  {

  }
};

class ItemDatabase
{
private:
  static Block parseBlock(std::vector<std::string> lines);
  static Item* parseItem(std::vector<std::string> lines);
  static std::map<std::string,std::string> compileDictionary(std::vector<std::string> lines);
  static void initStrToItemDictionary();
public:
  static Block blockDictionary[256];
  static std::vector<Item*> itemDictionary;
  static bool loadBlockDictionary(const char* file);
  static bool loadItemDictionary(const char* file);
};


#define BLOCKRES 128


enum Faces {FRONTF = 1 << 0, BACKF = 1 << 1,
            TOPF = 1 << 2, BOTTOMF = 1 << 3,
            RIGHTF = 1 << 4, LEFTF = 1 << 5};

class Block
{
public:

  std::string name;
  int id;
  uchar texArray[6];
  RenderType visibleType;
  bool isLightSource;
  glm::vec3 lightColor;
  int lightSize;
  Block(std::string newName, int newId, int* array,RenderType newVisibleType)
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
    for(int i =0;i<6;i++)
    {
      std::cout << (int) texArray[i] << ",";
    }
    std::cout << "\n";
  }

  uchar getFace(Faces face)
  {
    switch(face)
    {
      case(FRONTF): return getFront(); break;
      case(BACKF): return getBack(); break;
      case(BOTTOMF): return getBottom(); break;
      case(TOPF): return getTop(); break;
      case(LEFTF): return getLeft(); break;
      case(RIGHTF): return getRight(); break;
    }
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

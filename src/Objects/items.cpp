#include <iostream>
#include <sstream>
#include <fstream>
#include "include/items.h"

Block ItemDatabase::blockDictionary[256];
std::vector<Item*> ItemDatabase::itemDictionary;

Block ItemDatabase::parseBlock(std::vector<std::string> lines)
{
  using namespace std;
  map<string,string> dictionary = compileDictionary(lines);
  Block newBlock;
  newBlock.id = std::stoi(dictionary["id"]);
  newBlock.name = dictionary["name"];
  string textures = dictionary["textureIds"];
  stringstream ss;
  ss << textures;
  for(int i = 0;i<6;i++)
  {
    int texNumb;
    ss >> texNumb;
    newBlock.texArray[i] = texNumb;
    if(ss.peek() == ',') ss.ignore();
  }

  string opacity = dictionary["opacity"];
  if(opacity == "translucent") newBlock.visibleType = TRANSLUCENT;
  else if(opacity == "opaque") newBlock.visibleType = OPAQUE;
  else if(opacity == "transparent") newBlock.visibleType = TRANSPARENT;

  return newBlock;
}

std::map<std::string,std::string> ItemDatabase::compileDictionary(std::vector<std::string> lines)
{
  using namespace std;
  map<string,string> dictionary;

  for(auto itr = lines.begin(); itr != lines.end(); ++itr)
  {
    string line = *itr;
    int colonPos = line.find(':');
    if(colonPos == std::string::npos) continue;

    string key = line.substr(0,colonPos-1);
    string value = line.substr(colonPos+1,line.length());
    key.erase( std::remove_if( key.begin(), key.end(), ::isspace ), key.end() );
    value.erase( std::remove_if( value.begin(), value.end(), ::isspace ), value.end() );
    dictionary[key] = value;
  }
  return dictionary;
}

std::map<std::string,ItemType> strToItemDictionary;

void ItemDatabase::initStrToItemDictionary()
{
  strToItemDictionary["Placeable"] = PLACEABLE;
  strToItemDictionary["LightSource"] = LIGHTSOURCE;
  strToItemDictionary["Food"] = FOOD;
}


Item* ItemDatabase::parseItem(std::vector<std::string> lines)
{

  using namespace std;
  map<string,string> dictionary = compileDictionary(lines);
  Item* newItem;
  std::string type = dictionary["type"];
  switch(strToItemDictionary[type])
  {
    case LIGHTSOURCE:
      newItem = new LightSource; break;


    default:
      std::cout << "invalid item type" << type << "\n";
    return NULL;
  }


  newItem->id = std::stoi(dictionary["id"]);
  newItem->name = dictionary["name"];
  if(dictionary.count("range") == 1)
  {
    newItem->reach = std::stoi(dictionary["range"]);
  }
  if(dictionary.count("damage") == 1)
  {
    newItem->damage = std::stoi(dictionary["damage"]);
  }

  return newItem;

}

bool ItemDatabase::loadBlockDictionary(const char* filePath)
{
  using namespace std;
  ifstream file(filePath);

  if(!file.is_open())
  {
    std::cout << "Error opening dicionary at: " << filePath << "\n";
    return false;
  }

  string line;
  while(getline(file,line))
  {

    int brace = line.find('{');
    if(brace != std::string::npos)
    {
      vector<string> blockLines;
      while(getline(file,line))
      {
        int brace = line.find('}');
        if(brace != std::string::npos) break;

        blockLines.push_back(line);
      }
      Block newBlock = parseBlock(blockLines);
      blockDictionary[newBlock.id] = newBlock;
    }
  }
}

bool ItemDatabase::loadItemDictionary(const char* filePath)
{
  initStrToItemDictionary();
  itemDictionary.reserve(200);
  using namespace std;
  ifstream file(filePath);

  if(!file.is_open())
  {
    std::cout << "Error opening dicitonary at: " << filePath << "\n";
    return false;
  }

  string line;
  while(getline(file,line))
  {

    int brace = line.find('{');
    if(brace != std::string::npos)
    {
      vector<string> itemLines;
      while(getline(file,line))
      {
        int brace = line.find('}');
        if(brace != std::string::npos) break;

        itemLines.push_back(line);
      }
      Item* newItem = parseItem(itemLines);
      itemDictionary[newItem->id] = newItem;
    }
  }
}

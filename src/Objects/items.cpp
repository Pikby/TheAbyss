#include <iostream>
#include <sstream>
#include <fstream>
#include "include/items.h"

Block ItemDatabase::blockDictionary[256];
std::vector<Item*> ItemDatabase::itemDictionary;

void removeWhiteSpace(std::string& line)
{
  line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
}

glm::vec3 parseVec(std::string line)
{
  std::stringstream ss;
  ss << line;
  float arr[3];
  for(int i = 0;i<3;i++)
  {
    if(ss.peek() == ',') ss.ignore();
    ss >> arr[i];
    std::cout << arr[i] << "\n";

  }
  return glm::vec3(arr[0],arr[1],arr[2]);
}

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

  if(dictionary.count("isLightSource"))
  {
    newBlock.isLightSource = true;
    newBlock.lightColor = parseVec(dictionary["lightColor"]);
    newBlock.lightSize  = std::stoi(dictionary["lightSize"]);
  }


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
    if(colonPos == std::string::npos)
    {

      line.erase( std::remove_if(line.begin(),line.end(), ::isspace ), line.end() );
      if(line != "")
      {
        dictionary[line] = "true";
        continue;
      }
    }

    string key = line.substr(0,colonPos-1);
    string value = line.substr(colonPos+1,line.length());
    removeWhiteSpace(key);
    removeWhiteSpace(value);

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
      newItem = new LightSourceItem; break;


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

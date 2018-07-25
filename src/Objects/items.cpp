#include <iostream>
#include <sstream>
#include <fstream>
#include "include/items.h"

Block ItemDatabase::blockDictionary[256];
/*
int Item::mouse1(glm::vec3 pos, glm::vec3 front)
{
  int target = rayCast(pos,front);
  if(target == NOTHING)
  {
    return 0;
  }
  else if(target == BLOCK)
  {
    switch(Type)
      case :

  }
  else
  {

  }

}

ItemDatabase::ItemDatabase(const char* filePath)
{

}

*/



bool ItemDatabase::loadItemDictionary(const char* file)
{
  return false;
}

Block ItemDatabase::parseBlock(std::vector<std::string> lines)
{
  using namespace std;
  std::map<std::string,std::string> dictionary;
  for(auto itr = lines.begin(); itr != lines.end(); ++itr)
  {
    std::string line = *itr;
    int colonPos = line.find(':');
    if(colonPos == std::string::npos) continue;

    string key = line.substr(0,colonPos-1);
    string value = line.substr(colonPos+1,line.length());
    key.erase( std::remove_if( key.begin(), key.end(), ::isspace ), key.end() );
    value.erase( std::remove_if( value.begin(), value.end(), ::isspace ), value.end() );
    dictionary[key] = value;
  }
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

  for(int i = 0; i< 4;i++)
  {
    blockDictionary[i].print();
  }
  std::cout << "done lul\n";
}

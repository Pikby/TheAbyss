#include "../headers/all.h"
Block* ItemDatabase::blockDictionary;
GLuint ItemDatabase::textureAtlas;
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

bool ItemDatabase::loadBlockDictionary(const char* file)
{
  /*
  Loads the dictionary in such a format
  TODO: work on improving and discribing that format


  */
  blockDictionary = new Block[256];
  int id = 0;
  using namespace std;
  string line;
  ifstream dictionaryf (file);

  if(dictionaryf.is_open())
  {
    std::cout << "Loading dictionary\n";
    getline(dictionaryf,line);
    int atlasWidth = stoi(line);
    getline(dictionaryf,line);
    int atlasHeight = stoi(line);

    int curBlock=0;
    while(getline(dictionaryf,line))
    {
      if(line != "{") continue;
      getline(dictionaryf,line);
      string name = line;
      getline(dictionaryf,line);

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
      int visibleType = stoi(line);
      getline(dictionaryf,line);
      int width = stoi(line);
      getline(dictionaryf,line);
      int height = stoi(line);
      getline(dictionaryf,line);
      int depth = stoi(line);
      blockDictionary[curBlock] = Block(name,id++,texArray,visibleType,width,height,depth,atlasWidth,atlasHeight);
      curBlock++;
    }
    for(int i = 0; i< 4;i++)
    {
      blockDictionary[i].print();
    }

    return true;
  }
  else
  {
    std::cout <<"ERROR: block dictionary not found\n";
    return false;
  }


}

Block::Block(std::string newName,int newId, int* array, int newVisibleType,
            int newWidth,int newHeight,int newDepth, int newAtlasWidth, int newAtlasHeight)
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

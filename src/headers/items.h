enum Type {NONE,MELEEWEAPON,RANGEDWEAPON};

class Item : public WorldWrap
{
  std::string name;
  int type;
  int reach = 3;
  int damage = 0.5;

  int leftClick(glm::vec3 pos, glm::vec3 front);
  int rightClick();
};

class ItemDatabase
{
  unsigned short int* dictionary
  GLuint textureAtlas;
  ItemDatabase(const char* filePath);
};

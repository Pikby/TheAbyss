


class Block
{
public:
  GLuint VBO, EBO, VAO;
  GLuint glTexture;
  Shader* blockShader;
  const char* texture;
  //0 = STATIC, 1 = DYNAMIC, 2 = STREAM
  int type;
  Block(const char* newTexture, int newType);
  void refresh();
};

class WorldBlk
{
public:
  float xpos, ypos,zpos;
  Block* id;
  WorldBlk(int x, int y, int z, Block* newId);
  void draw(glm::mat4 camera);
};

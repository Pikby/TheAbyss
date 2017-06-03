
enum Type {STATIC,DYNAMIC,STREAM};

class Block
{
private:
  GLuint VBO, VAO, EBO;
  GLuint glTexture;
  Shader* blockShader;
public:
  float xpos;
  float ypos;
  const char* texture;
  //0 = STATIC, 1 = DYNAMIC, 2 = STREAM
  int type;
  Block(float x, float y, const char* newTexture, int newType);

  void draw(glm::mat4 camera);
  void refresh();
};

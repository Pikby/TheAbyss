
enum Type {STATIC,DYNAMIC,STREAM};

class block
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
  block(float x, float y, const char* newTexture, int newType);

  void draw();
  void refresh();
};

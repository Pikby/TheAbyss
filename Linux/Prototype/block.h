

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
  block(float x, float y, const char* newTexture);

  void draw();
  void init();
};

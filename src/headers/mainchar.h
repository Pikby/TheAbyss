#pragma once

class MainChar
{
protected:

public:
  GLuint VBO, VAO, EBO;
  GLuint glTexture;
  Shader* mainCharShader;
  float xpos;
  float ypos;
  const char* texture;
  //0 = STATIC, 1 = DYNAMIC, 2 = STREAM
  int type;
  MainChar(float x, float y, const char* newTexture);
  MainChar(const char* newTexture, int newType);
  void draw(glm::mat4 camera);
  void refresh();
  void update(std::vector <Block> blkArray);
};

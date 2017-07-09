#pragma once

class MainChar
{
private:
  GLuint VBO, VAO, EBO;
  GLuint glTexture;
  Shader* mainCharShader;
  std::vector<WorldBlk> *blkArray;

  float gravity;
  float maxSpeed;
  float deltax;
  float deltay;
  bool grounded;
public:
  float xpos;
  float ypos;
  const char* texture;
  //0 = STATIC, 1 = DYNAMIC, 2 = STREAM
  int type;
  MainChar(float x, float y, const char* newTexture, std::vector<WorldBlk> *newArray);
  void draw(glm::mat4 camera);
  void refresh();
  void update();
  void moveLeft();
  void moveRight();
  void jump();
};

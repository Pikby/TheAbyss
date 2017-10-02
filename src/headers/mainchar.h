#pragma once

class MainChar
{
private:
  GLuint VBO, VAO, EBO;
  GLuint glTexture;
  Shader* mainCharShader;
  World* curWorld;

  float gravity;
  float moveSpeed;
  float deltax;
  float deltay;
  float deltaz;
  bool grounded;
public:
  Camera mainCam;
  float xpos;
  float ypos;
  float zpos;
  //0 = STATIC, 1 = DYNAMIC, 2 = STREAM
  int type;
  MainChar(float x, float y, float z, World* curWorld );
  void update();
  void moveLeft();
  void moveRight();
  void moveForward();
  void moveBackward();
  void moveDown();
  void moveUp();
  void jump();
};

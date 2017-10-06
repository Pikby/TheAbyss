#pragma once

class MainChar : public WorldWrapper
{
private:
  GLuint VBO, VAO, EBO;
  GLuint glTexture;
  Shader* mainCharShader;
  World* curWorld;
  float gravity;
  float moveSpeed;
  float deltax,deltay,deltaz;
  glm::vec2 topLeftAction,bottomRightAction;
  bool grounded;
public:
  Camera mainCam;
  float xpos,ypos,zpos;
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
  void calculateHud();
  void drawHud();
};

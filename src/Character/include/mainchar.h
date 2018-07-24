#pragma once
#include "gui.h"
#include <atomic>
#include "../../headers/camera.h"
class World;

class Item;
class MainChar
{
  struct ActionBar
  {
    glm::vec2 topLeft,bottomRight;
    int width;
    unsigned int selected;
  };
private:
  //References
  ActionBar actionMain;
  World* curWorld;
  GUIRenderer gui;
  int* inventory;
  Item** dictionary;
  //Rendering objects
  GLuint VBO, VAO, EBO;
  GLuint glTexture;
  Shader mainCharShader;
  int screenWidth, screenHeight;

  //Character attributes
  float gravity;
  float moveSpeed;
  float deltax,deltay,deltaz;
  int reach = 200;
  bool grounded;
public:
  Camera mainCam;


  std::atomic<float> xpos,ypos,zpos;

  MainChar(float x, float y, float z, World* curWorld );
  //Update movement
  void update();

  //Movement inputs
  void moveLeft();
  void moveRight();
  void moveForward();
  void moveBackward();
  void moveDown();
  void moveUp();
  void jump();

  void addBlock(int id);
  void destroyBlock();
  void setPosition(float x, float y, float z);
  void draw();
  //Hud functions
  void calculateHud();
  void drawHud();
  void showFPS();
};

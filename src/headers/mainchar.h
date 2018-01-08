#pragma once

class Item;
class MainChar : public WorldWrap
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

  //Character attributes
  float gravity;
  float moveSpeed;
  float deltax,deltay,deltaz;
  int reach = 200;
  bool grounded;
public:
  Camera mainCam;


  float xpos,ypos,zpos;

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
  //Hud functions
  void calculateHud();
  void drawHud();
  void showFPS();
};

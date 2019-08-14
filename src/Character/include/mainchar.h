#ifndef MAINCHARLIBRARY
#define MAINCHARLIBRARY
#include <atomic>
#include <map>
#include <set>
#include "../../headers/camera.h"

class Item;
class MainChar
{
private:
  static std::set<int> keySet;
  static std::string userName;

  static Camera mainCam;

  //Character attributes
  static float gravity;
  static float moveSpeed;

  static glm::vec3 deltaPos;
  static glm::vec3 playerPos;
  static uint8_t heldItem;

  static bool isGrounded;
  static int reach;


public:

  static void initMainChar(const glm::vec3& pos);
  //Update movement
  static void update();
  static void processMouseMovement(float xoffset, float yoffset);

  //All the character actions
  static void moveLeft();
  static void moveRight();
  static void moveForward();
  static void moveBackward();
  static void moveDown();
  static void moveUp();
  static void jump();

  //Takes the key inputs to be mapped
  static void handleKeyPress(int key);
  static void handleKeyHold(int key);
  static void handleKeyRelease(int key);
  static void handleMouseClick(int key);

  //Maniuplates block infront of the character
  static void addBlock(int id);
  static void destroyBlock();
  static void setPosition(const glm::vec3 &pos);
  static void draw();

  static void drawPreviewBlock(Shader* shader);

  //Setters and Getters
  static Camera& getCamera(){return mainCam;}
  static uint8_t getHeldItem(){return heldItem;}
  static void setHeldItem(uint8_t id){heldItem = id;}
  static glm::vec3 getPosition(){return playerPos;}
  static glm::vec3 getViewDirection(){return mainCam.front;}
};


#ifdef MAINCHARIMPLEMENTATION
  std::string MainChar::userName;
  float MainChar::gravity, MainChar::moveSpeed = 0.2f;
  int MainChar::reach = 200;
  Camera MainChar::mainCam;
  glm::vec3 MainChar::playerPos, MainChar::deltaPos;
  uint8_t MainChar::heldItem=1;
  bool MainChar::isGrounded;
  std::set<int> MainChar::keySet;
#endif
#endif

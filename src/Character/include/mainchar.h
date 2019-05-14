#ifndef MAINCHARLIBRARY
#define MAINCHARLIBRARY
#include "gui.h"
#include <atomic>
#include "../../headers/camera.h"

class Item;


//enum Menu {WORLDMENU,INVENTORYMENU,OPTIONSMENU,CHATMENU};


class MainChar
{
private:
  static std::map<int,int> keyMap;

  static std::string userName;

  static int screenWidth, screenHeight;


  //Character attributes
  static float gravity;
  static float moveSpeed;
  static float deltax,deltay,deltaz;
  static int reach;
  static bool shiftHeld;
  static bool grounded;
  static bool controlHeld;


  static bool inInventory;
public:
  static char heldItem;
  static Camera mainCam;
  static std::atomic<float> xpos,ypos,zpos;

  static void initMainChar(float x, float y, float z);
  //Update movement
  static void update();
  static void processMouseMovement(float xoffset, float yoffset);
  static void switchInventoryMode();
  static void openChat();
  static void closeChat();
  static void sendMessage();
  //Movement inputs
  static void moveLeft();
  static void moveRight();
  static void moveForward();
  static void moveBackward();
  static void moveDown();
  static void moveUp();
  static void jump();

  static void handleKeyPress(int key);
  static void handleKeyHold(int key);
  static void handleKeyRelease(int key);
  static void handleMouseClick(int key);
  static void addBlock(int id);
  static void destroyBlock();
  static void setPosition(const glm::vec3 &pos);
  static void draw();
  //Hud functions
  static void addCharacterToChat(int key);
};
#ifdef MAINCHARIMPLEMENTATION
  std::string MainChar::userName;
  int MainChar::screenWidth, MainChar::screenHeight;
  float MainChar::gravity, MainChar::moveSpeed = 0.2f,MainChar::deltax = 0,MainChar::deltay = 0,MainChar::deltaz = 0;
  int MainChar::reach = 200;
  bool MainChar::shiftHeld = false, MainChar::grounded,MainChar::controlHeld,MainChar::inInventory;
  Camera MainChar::mainCam;
  std::atomic<float> MainChar::xpos,MainChar::ypos,MainChar::zpos;
  char MainChar::heldItem;
  std::map<int,int> MainChar::keyMap;
#endif
#endif

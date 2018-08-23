#pragma once
#include "gui.h"
#include <atomic>
#include "../../headers/camera.h"
class World;

class Item;


enum Menu {WORLDMENU,INVENTORYMENU,OPTIONSMENU,CHATMENU};


class MainChar
{
private:
  //References
  std::string userName;
  Menu curMenu;
  World* curWorld;
  GUIRenderer gui;
  int screenWidth, screenHeight;
  std::deque<std::string> chatLog;

  //Character attributes
  float gravity;
  float moveSpeed;
  float deltax,deltay,deltaz;
  int reach = 200;
  bool grounded;
  bool shiftHeld;
  bool controlHeld;


  bool inInventory;
public:
  Camera mainCam;


  std::atomic<float> xpos,ypos,zpos;

  MainChar(float x, float y, float z, World* curWorld );
  //Update movement
  void update();
  void processMouseMovement(float xoffset, float yoffset);
  void switchInventoryMode();
  void openChat();
  void closeChat();
  void sendMessage();
  //Movement inputs
  void moveLeft();
  void moveRight();
  void moveForward();
  void moveBackward();
  void moveDown();
  void moveUp();
  void jump();

  void handleKeyPress(int key);
  void handleKeyHold(int key);
  void handleKeyRelease(int key);
  void handleMouseClick(int key);
  void addBlock(int id);
  void destroyBlock();
  void setPosition(const glm::vec3 &pos);
  void draw();
  //Hud functions
  void addCharacterToChat(int key);
  void addChatLine(std::string line)
  {
    gui.chatConsole.incomingMessages->push(line);
  }
};

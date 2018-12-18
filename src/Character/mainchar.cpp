#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include "../MainEngine/include/world.h"
#include "../headers/shaders.h"
#include "../Settings/settings.h"
#include "include/mainchar.h"

#define PI 3.14159265
std::string MainChar::userName;
Menu MainChar::curMenu;
GUIRenderer MainChar::gui;
int MainChar::screenWidth, MainChar::screenHeight;
std::deque<std::string> MainChar::chatLog;
float MainChar::gravity, MainChar::moveSpeed,MainChar::deltax,MainChar::deltay,MainChar::deltaz;
int MainChar::reach;
bool MainChar::shiftHeld, MainChar::grounded,MainChar::controlHeld,MainChar::inInventory;
Camera MainChar::mainCam;
std::atomic<float> MainChar::xpos,MainChar::ypos,MainChar::zpos;


template <typename T> int sign(T val)
{
  return (T(0) < val) - (val < T(0));
}

float max(float a, float b, float c)
{
  float m = a;
  if(m<b) m = b;
  if(m<c) m = c;
  return m;

}

void MainChar::initMainChar(float x, float y, float z)
{
  shiftHeld = false;
  curMenu = WORLDMENU;
  xpos = x;
  ypos = y;
  zpos = z;
  deltax = 0;
  deltay = 0;
  deltaz = 0;
  reach = 200;
  moveSpeed = 1.0f;
  mainCam = Camera(glm::vec3((float)xpos,(float)ypos,(float)zpos));
  screenWidth = World::drawer.screenWidth;
  screenHeight = World::drawer.screenHeight;
  userName = Settings::get("userName");
  gui = GUIRenderer(screenWidth,screenHeight,userName);


}
void MainChar::handleKeyHold(int key)
{
  switch(curMenu)
  {
    case CHATMENU: break;
    default:
      switch(key)
      {
        case GLFW_KEY_W:			moveForward(); 					break;
        case GLFW_KEY_Q:		  moveDown();							break;
        case GLFW_KEY_A:			moveLeft();							break;
        case GLFW_KEY_D:			moveRight();						break;
        case GLFW_KEY_S:			moveBackward();					break;
        case GLFW_KEY_SPACE:	moveUp();								break;
      }
      break;
  }
}

void MainChar::handleKeyRelease(int key)
{
  switch(key)
  {
    case GLFW_KEY_LEFT_SHIFT : shiftHeld = false; break;
    case GLFW_KEY_LEFT_CONTROL : controlHeld = false; break;
  }
}

void MainChar::handleKeyPress(int key)
{
  //guiRenderer.handleKeyPress(int key);
  //Universal keys
  switch(key)
  {
    case GLFW_KEY_LEFT_SHIFT:   shiftHeld = true; return; break;
    case GLFW_KEY_LEFT_CONTROL: controlHeld = true; return; break;
  }

  switch(curMenu)
  {
    case CHATMENU:
      switch(key)
      {
        case GLFW_KEY_ESCAPE: closeChat(); break;
        case GLFW_KEY_ENTER: sendMessage(); break;
        default: addCharacterToChat(key); break;
      }
    default:
      switch(key)
      {
        case GLFW_KEY_N: std::cout << xpos << ":" << ypos << ":" << zpos << "\n"; break;
        case GLFW_KEY_E: switchInventoryMode(); break;
        case GLFW_KEY_T: openChat(); break;
      }
      break;

  }
}

void MainChar::addCharacterToChat(int key)
{
  gui.chatConsole.addCharacterToChat(key,shiftHeld);
}

void MainChar::update()
{
  if(!World::blockExists(glm::ivec3(floor(xpos+deltax),floor(ypos),floor(zpos))))
  {
    xpos = xpos + deltax;
  }

  if(!World::blockExists(glm::ivec3(floor(xpos),floor(ypos),floor(zpos+deltaz))))
  {
    zpos = zpos + deltaz;
  }

  if(!World::blockExists(glm::ivec3(floor(xpos),floor(ypos+deltay),floor(zpos))))
  {
    ypos = ypos + deltay;
    grounded = false;
  }
  deltax /= 5;
  deltay /= 5;
  deltaz /= 5;
  mainCam.setPosition((float)xpos,(float)ypos,(float)zpos);
  gui.chatConsole.update();

  static double lastFrame = 0;
  static double curFrame = 0;
  static double lastfps = 0;
  curFrame = glfwGetTime();
  double curfps = 1.0f/(curFrame - lastFrame);
  const double smoothing = 0.999;
  double fps = lastfps*smoothing + curfps*(1-smoothing);
  lastFrame = curFrame;
  lastfps = fps;
  std::string line = "[colour='FF000000']FPS: " + std::to_string(fps);
  gui.gameWindow->getChild("FPS")->setText(line);
  line = "[colour='FF000000']ChunksLoaded: " + std::to_string(BSPNode::totalChunks);
  gui.gameWindow->getChild("Chunks")->setText(line);
  line = "[colour='FF000000']Chunks In Queue 0: " + std::to_string(World::buildQueue.size());
  gui.gameWindow->getChild("ChunksQueue")->setText(line);
}

void MainChar::moveRight()
{
  deltax += cos((mainCam.yaw+90)*PI/180.0)*moveSpeed;
  deltaz += sin((mainCam.yaw+90)*PI/180.0)*moveSpeed;
}

void MainChar::moveLeft()
{
  deltax += cos((mainCam.yaw-90)*PI/180.0)*moveSpeed;
  deltaz += sin((mainCam.yaw-90)*PI/180.0)*moveSpeed;
}

void MainChar::moveForward()
{
  deltax += cos(mainCam.yaw*PI/180.0)*moveSpeed;
  deltaz += sin(mainCam.yaw*PI/180.0)*moveSpeed;
}

void MainChar::moveBackward()
{
  deltax += -cos(mainCam.yaw*PI/180.0)*moveSpeed;
  deltaz += -sin(mainCam.yaw*PI/180.0)*moveSpeed;
}

void MainChar::setPosition(const glm::vec3 &pos)
{
  if(pow(pos.x-xpos,2)+pow(pos.y-ypos,2)+pow(pos.z-zpos,2) < 64*64) return;
  xpos = pos.x;
  ypos = pos.y;
  zpos = pos.z;
}

void MainChar::moveDown()
{
  deltay += -moveSpeed;
}

void MainChar::moveUp()
{
  deltay += moveSpeed;
}

void MainChar::jump()
{
  if(grounded) deltay = 5.0f;
}
//Destroys the block ur looking at
void MainChar::destroyBlock()
{
  glm::vec4 block = World::rayCast(mainCam.position,mainCam.front,reach);

  if(block.w == NOTHING) return;
  std::cout << "Destroying blocks\n";
  World::messenger.createDelBlockRequest(floor(block.x),floor(block.y),floor(block.z));

}
void MainChar::addBlock(int id)
{
    glm::vec4 block = World::rayCast(mainCam.position,mainCam.front,reach);
    if(block.w == NOTHING) return;

    glm::vec3 p1 = glm::vec3(block);
    glm::vec3 p2 = p1 - mainCam.front/10.0f;

    int x = floor(p1.x)-floor(p2.x);
    int y = floor(p1.y)-floor(p2.y);
    int z = floor(p1.z)-floor(p2.z);

    if(x != 0)
    {
      World::messenger.createAddBlockRequest(floor(p1.x)-sign(x),floor(p1.y),floor(p1.z),id);
    }
    else if(y != 0)
    {
      World::messenger.createAddBlockRequest(floor(p1.x),floor(p1.y)-sign(y),floor(p1.z),id);
    }
    else if(z != 0)
    {
      World::messenger.createAddBlockRequest(floor(p1.x),floor(p2.y),floor(p1.z)-sign(z),id);
    }

}

void MainChar::processMouseMovement(float xoffset, float yoffset)
{
  switch(curMenu)
  {
    case WORLDMENU: mainCam.processMouseMovement(xoffset,yoffset,true); break;
    case INVENTORYMENU:
    {
      CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
      context.injectMouseMove(xoffset,-yoffset);
    }
  }

}
void MainChar::handleMouseClick(int key)
{
  if(curMenu!= WORLDMENU) return;
  switch(key)
  {
    case GLFW_MOUSE_BUTTON_LEFT: destroyBlock() ; break;
    case GLFW_MOUSE_BUTTON_RIGHT:  addBlock(4); break;

  }

}

void MainChar::switchInventoryMode()
{
  if(curMenu == INVENTORYMENU)
  {
    gui.closeInventoryGUI();
    curMenu = WORLDMENU;
  }
  else
  {
    gui.openInventoryGUI();
    curMenu = INVENTORYMENU;
  }
}

void MainChar::openChat()
{
  curMenu = CHATMENU;
  gui.chatConsole.open(true);
}

void MainChar::closeChat()
{
  curMenu = WORLDMENU;
  gui.chatConsole.open(false);
}

void MainChar::sendMessage()
{
  curMenu = WORLDMENU;
  gui.chatConsole.open(false);
  if(gui.chatConsole.curMsg != "")
  {
    World::messenger.createChatMessage(gui.chatConsole.curMsg);
    gui.chatConsole.sendCurrentMessage();
  }

}

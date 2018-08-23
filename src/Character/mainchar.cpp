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

MainChar::MainChar(float x, float y, float z, World* world )
{
  shiftHeld = false;
  curMenu = WORLDMENU;
  xpos = x;
  ypos = y;
  zpos = z;
  deltax = 0;
  deltay = 0;
  deltaz = 0;
  moveSpeed = 1.0f;
  curWorld = world;
  mainCam = Camera(glm::vec3((float)xpos,(float)ypos,(float)zpos));
  screenWidth = curWorld->drawer.screenWidth;
  screenHeight = curWorld->drawer.screenHeight;
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
  if(!curWorld->blockExists(glm::ivec3(floor(xpos+deltax),floor(ypos),floor(zpos))))
  {
    xpos = xpos + deltax;
  }

  if(!curWorld->blockExists(glm::ivec3(floor(xpos),floor(ypos),floor(zpos+deltaz))))
  {
    zpos = zpos + deltaz;
  }

  if(!curWorld->blockExists(glm::ivec3(floor(xpos),floor(ypos+deltay),floor(zpos))))
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
  curFrame = glfwGetTime();
  double fps = 1.0f/(curFrame - lastFrame);
  lastFrame = curFrame;
  std::string line = "FPS: " + std::to_string(fps);
  gui.gameWindow->getChild("FPS")->setText(line);
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
  glm::vec4 block = curWorld->rayCast(mainCam.position,mainCam.front,reach);

  if(block.w == NOTHING) return;
  std::cout << "Destroying blocks\n";
  curWorld->messenger.createDelBlockRequest(floor(block.x),floor(block.y),floor(block.z));

} 
void MainChar::addBlock(int id)
{
    glm::vec4 block = curWorld->rayCast(mainCam.position,mainCam.front,reach);
    if(block.w == NOTHING) return;

    glm::vec3 p1 = glm::vec3(block);
    glm::vec3 p2 = p1 - mainCam.front/10.0f;

    int x = floor(p1.x)-floor(p2.x);
    int y = floor(p1.y)-floor(p2.y);
    int z = floor(p1.z)-floor(p2.z);

    if(x != 0)
    {
      curWorld->messenger.createAddBlockRequest(floor(p1.x)-sign(x),floor(p1.y),floor(p1.z),id);
    }
    else if(y != 0)
    {
      curWorld->messenger.createAddBlockRequest(floor(p1.x),floor(p1.y)-sign(y),floor(p1.z),id);
    }
    else if(z != 0)
    {
      curWorld->messenger.createAddBlockRequest(floor(p1.x),floor(p2.y),floor(p1.z)-sign(z),id);
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
    case GLFW_MOUSE_BUTTON_RIGHT:  addBlock(1); break;

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
    curWorld->messenger.createChatMessage(gui.chatConsole.curMsg);
    gui.chatConsole.sendCurrentMessage();
  }

}

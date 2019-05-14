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

#define MAINCHARIMPLEMENTATION
#include "include/mainchar.h"
#include "../MainEngine/imgui/imgui.h"
#define PI 3.14159265


template <typename T> int sign(T val)
{
  return (T(0) < val) - (val < T(0));
}


void MainChar::initMainChar(float x, float y, float z)
{

  xpos = x;
  ypos = y;
  zpos = z;
  mainCam = Camera(glm::vec3((float)xpos,(float)ypos,(float)zpos));
  screenWidth = World::drawer.screenWidth;
  screenHeight = World::drawer.screenHeight;
  userName = Settings::get("userName");


}
void MainChar::handleKeyHold(int key)
{

      switch(key)
      {
        case GLFW_KEY_W:			moveForward(); 					break;
        case GLFW_KEY_LEFT_SHIFT:		  moveDown();							break;
        case GLFW_KEY_A:			moveLeft();							break;
        case GLFW_KEY_D:			moveRight();						break;
        case GLFW_KEY_S:			moveBackward();					break;
        case GLFW_KEY_SPACE:	moveUp();								break;
      }


}

void MainChar::handleKeyRelease(int key)
{
  keyMap.erase(key);
}

void MainChar::handleKeyPress(int key)
{
	keyMap[key] = GLFW_PRESS;

}

void MainChar::addCharacterToChat(int key)
{

}

void MainChar::update()
{
  for(auto it = keyMap.begin();it!= keyMap.end();++it)
  {
    handleKeyHold(it->first);
  }

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
}  static char heldItem=1;

void MainChar::jump()
{
  if(grounded) deltay = 5.0f;
}
//Destroys the block ur looking at
void MainChar::destroyBlock()
{
  glm::vec4 block = World::rayCast(mainCam.position,mainCam.front,reach);

  if(block.w == NOTHING) return;

/*
  glm::vec3 pos = floor(block);
  glm::vec3 norm = mainCam.position - block;
  glm::vec3 dir;

  if(norm.x >= norm.y && norm.x >= norm.z)
  {
    dir = norm.x > 0 ? glm::vec3(1,0,0) : glm::vec3(-1,0,0);
  }
  else if(norm.y >= norm.x && norm.y >= norm.z)
  {
    dir = norm.y > 0 ? glm::vec3(0,1,0) : glm::vec3(0,-1,0);
  }
  else if(norm.z >= norm.y && norm.z >= norm.x)
  {
    dir = norm.z > 0 ? glm::vec3(0,0,1) : glm::vec3(0,0,-1);
  }
*/


  std::cout << "Destroying blocks\n";
  World::messenger.createDelBlockRequest(floor(block.x),floor(block.y),floor(block.z));

/*
  for(int x = -1; x<=1 ;x++)
  {
    for(int y = -1; y<=1 ;y++)
    {
      for(int z = -1; z<= 1;z++)
      {
        World::messenger.createDelBlockRequest(floor(block.x+x),floor(block.y+y),floor(block.z+z));
      }
    }
  }
*/
}
void MainChar::addBlock(int id)
{
    glm::vec4 block = World::rayCast(mainCam.position,mainCam.front,reach);
    if(block.w == NOTHING) return;

    glm::vec3 p1 = glm::vec3(block);
    glm::vec3 p2 = p1 - glm::vec3(mainCam.front)/10.0f;

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
  mainCam.processMouseMovement(xoffset,yoffset,true);
}
void MainChar::handleMouseClick(int key)
{
  switch(key)
  {
    case GLFW_MOUSE_BUTTON_LEFT: destroyBlock() ; break;
    case GLFW_MOUSE_BUTTON_RIGHT:  addBlock(heldItem); break;

  }

}

void MainChar::switchInventoryMode()
{

}

void MainChar::openChat()
{

}

void MainChar::closeChat()
{

}

void MainChar::sendMessage()
{

}

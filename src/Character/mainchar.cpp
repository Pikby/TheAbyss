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


void MainChar::initMainChar(const glm::vec3& pos)
{
  playerPos = pos;
  mainCam = Camera(playerPos);
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

  if(!World::blockExists(glm::ivec3(floor(playerPos.x+deltaPos.x),floor(playerPos.y),floor(playerPos.z))))
  {
    playerPos.x = playerPos.x + deltaPos.x;
  }

  if(!World::blockExists(glm::ivec3(floor(playerPos.x),floor(playerPos.y),floor(playerPos.z+deltaPos.z))))
  {
    playerPos.z = playerPos.z + deltaPos.z;
  }

  if(!World::blockExists(glm::ivec3(floor(playerPos.x),floor(playerPos.y+deltaPos.y),floor(playerPos.z))))
  {
    playerPos.y = playerPos.y + deltaPos.y;
    isGrounded = false;
  }
  deltaPos /= 5.0f;
  mainCam.setPosition(playerPos);
}

void MainChar::moveRight()
{
  deltaPos.x += cos((mainCam.yaw+90)*PI/180.0)*moveSpeed;
  deltaPos.z += sin((mainCam.yaw+90)*PI/180.0)*moveSpeed;
}

void MainChar::moveLeft()
{
  deltaPos.x += cos((mainCam.yaw-90)*PI/180.0)*moveSpeed;
  deltaPos.z += sin((mainCam.yaw-90)*PI/180.0)*moveSpeed;
}

void MainChar::moveForward()
{
  deltaPos.x += cos(mainCam.yaw*PI/180.0)*moveSpeed;
  deltaPos.z += sin(mainCam.yaw*PI/180.0)*moveSpeed;
}

void MainChar::moveBackward()
{
  deltaPos.x += -cos(mainCam.yaw*PI/180.0)*moveSpeed;
  deltaPos.z += -sin(mainCam.yaw*PI/180.0)*moveSpeed;
}

void MainChar::setPosition(const glm::vec3 &pos)
{
  if(pow(pos.x-playerPos.x,2)+pow(pos.y-playerPos.y,2)+pow(pos.z-playerPos.z,2) < 64*64) return;
  playerPos.x = pos.x;
  playerPos.y = pos.y;
  playerPos.z = pos.z;
}

void MainChar::moveDown()
{
  deltaPos.y += -moveSpeed;
}

void MainChar::moveUp()
{
  deltaPos.y += moveSpeed;
}  static char heldItem=1;

void MainChar::jump()
{
  if(isGrounded) deltaPos.y = 5.0f;
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

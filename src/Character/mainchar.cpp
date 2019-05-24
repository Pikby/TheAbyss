#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <map>
#include <string>

#include "../MainEngine/include/world.h"
#include "../headers/shaders.h"
#include "../Settings/settings.h"

#define MAINCHARIMPLEMENTATION
#include "include/mainchar.h"
#include "../MainEngine/imgui/imgui.h"



template <typename T> int sign(T val)
{
  return (T(0) < val) - (val < T(0));
}


void MainChar::initMainChar(const glm::vec3& pos)
{
  playerPos = pos;
  mainCam = Camera(playerPos);
}

void MainChar::handleKeyHold(int key)
{
  switch(key)
  {
    case GLFW_KEY_W:			        moveForward(); 					break;
    case GLFW_KEY_LEFT_SHIFT:		  moveDown();							break;
    case GLFW_KEY_A:			        moveLeft();							break;
    case GLFW_KEY_D:			        moveRight();						break;
    case GLFW_KEY_S:			        moveBackward();					break;
    case GLFW_KEY_SPACE:	        moveUp();								break;

  }
}

void MainChar::handleKeyRelease(int key)
{
  keySet.erase(key);
}

void MainChar::handleKeyPress(int key)
{
  keySet.insert(key);
  if(key == GLFW_KEY_P) World::messenger.createPingRequest();
}

void MainChar::update()
{

  static double lastMoveTime = glfwGetTime();
  static double lastSendTime = glfwGetTime();
  const double movementSendRate = 1.0/10.0;
  const double movementUpdateRate = 1.0/60.0;
  double curTime = glfwGetTime();


  if(curTime - lastSendTime > movementSendRate)
  {
    World::messenger.createMoveRequest(playerPos.x,playerPos.y,playerPos.z);
    World::messenger.createViewDirectionChangeRequest(mainCam.front.x,mainCam.front.y,mainCam.front.z);
  }


  if(curTime-lastMoveTime < movementUpdateRate)
  {
    return;
  }
  lastMoveTime = curTime;

  for(auto it = keySet.begin();it!= keySet.end();++it)
  {
    handleKeyHold(*it);
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
  deltaPos.x += cos(glm::radians(mainCam.yaw+90))*moveSpeed;
  deltaPos.z += sin(glm::radians(mainCam.yaw+90))*moveSpeed;
}

void MainChar::moveLeft()
{
  deltaPos.x += cos(glm::radians(mainCam.yaw-90))*moveSpeed;
  deltaPos.z += sin(glm::radians(mainCam.yaw-90))*moveSpeed;
}

void MainChar::moveForward()
{
  deltaPos.x += cos(glm::radians(mainCam.yaw))*moveSpeed;
  deltaPos.z += sin(glm::radians(mainCam.yaw))*moveSpeed;
}

void MainChar::moveBackward()
{
  deltaPos.x += -cos(glm::radians(mainCam.yaw))*moveSpeed;
  deltaPos.z += -sin(glm::radians(mainCam.yaw))*moveSpeed;
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
}

void MainChar::jump()
{
  if(isGrounded) deltaPos.y = 5.0f;
}

void MainChar::destroyBlock()
{
  glm::vec4 block = World::rayCast(mainCam.position,mainCam.front,reach);
  if(block.w == NOTHING) return;
  World::messenger.createDelBlockRequest(floor(block.x),floor(block.y),floor(block.z));
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

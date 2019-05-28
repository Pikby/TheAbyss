#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../include/gui.h"
#include "../include/mainchar.h"
#include "widgets.h"

void InGame::draw()
{
  GUI::drawQuad(glm::vec2(0.5)-glm::vec2(0.001),glm::vec2(0.5)+glm::vec2(0.001),glm::vec4(0.7,0.7,0.7,1));
}


void InGame::handleKeyInput(int key,int action)
{
  if(action == GLFW_PRESS || action == GLFW_REPEAT)
  {
    MainChar::handleKeyPress(key);
  }
  else if(action == GLFW_RELEASE)
  {
    MainChar::handleKeyRelease(key);
  }

  //std::cout << key << "\n";
}

void InGame::handleCharInput(uint character)
{

}

void InGame::handleScrollInput(double xoffset,double yoffset)
{

}

void InGame::handleMouseMovement(double xpos, double ypos)
{
  static float lastX,lastY;
  static bool firstMouse = true;
  if (firstMouse)
  {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//std::cout << xpos << ":" << ypos << "\n";
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;
	MainChar::processMouseMovement(xoffset, yoffset);

}

void InGame::handleMouseInput(int button,int action)
{
  if(action == GLFW_PRESS) MainChar::handleMouseClick(button);
}

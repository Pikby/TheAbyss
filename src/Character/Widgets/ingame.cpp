#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../include/mainchar.h"
#include "../../MainEngine/imgui/imgui.h"
#include "widgets.h"

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

  ImGuiIO& io = ImGui::GetIO();
  io.MousePos = ImVec2(xpos,ypos);
}

void InGame::handleMouseInput(int button,int action)
{
  if(action == GLFW_PRESS) MainChar::handleMouseClick(button);
}

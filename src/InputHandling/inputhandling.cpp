#define GLEW_STATIC
#include <GL/glew.h>
#include <map>
// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <math.h>

#include "../Character/include/mainchar.h"
#include "../MainEngine/include/world.h"
#include "../MainEngine/include/threads.h"
#include "../Settings/settings.h"
#include "../MainEngine/imgui/imgui.h"
// timing
static float deltaTime = 0.0f;	// time between current frame and last frame
static float lastFrame = 0.0f;

// mouse position variables
static float lastX = 400;
static float lastY = 300;
static bool firstMouse = true;
std::map<int,int> keyMap;


int inMenu = false;
void updateInputs()
{
	for(std::map<int,int>::iterator it=keyMap.begin(); it!=keyMap.end(); ++it)
	{

		int key = it->first;
		if(!inMenu) MainChar::handleKeyHold(key);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if(inMenu)
		{
			inMenu = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			inMenu = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	if(action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if(!inMenu) MainChar::handleKeyPress(key);
	}
	else if(action == GLFW_RELEASE)
	{
		if(!inMenu) MainChar::handleKeyRelease(key);
	}

	if(keyMap.count(key) == 1)
	{
		if(action == GLFW_RELEASE)
		{
			keyMap.erase(key);
		}
	}
	else
	{
		keyMap[key] = action;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

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

	ImGuiIO& io = ImGui::GetIO();
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	if(inMenu)io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
	else MainChar::processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(!inMenu) MainChar::mainCam.processMouseScroll(yoffset);
}

void mousekey_callback(GLFWwindow* window, int button, int action, int mods)
{

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
    if(!inMenu) MainChar::handleMouseClick(GLFW_MOUSE_BUTTON_LEFT);
	}
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
	}

  if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if(!inMenu) MainChar::handleMouseClick(GLFW_MOUSE_BUTTON_RIGHT);
	}
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
	}
}

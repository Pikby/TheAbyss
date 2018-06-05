#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <map>
#include <glm/glm.hpp>
#include "../headers/mainchar.h"

#include "../headers/threads.h"
// timing
static float deltaTime = 0.0f;	// time between current frame and last frame
static float lastFrame = 0.0f;

// mouse position variables
static float lastX = 400;
static float lastY = 300;
static bool firstMouse = true;
static MainChar* mainCharacter;
std::map<int,int> keyMap;


void initializeInputs(MainChar* mc)
{
	mainCharacter = mc;
}

void updateInputs()
{
	for(std::map<int,int>::iterator it=keyMap.begin(); it!=keyMap.end(); ++it)
	{
		switch(it->first)
		{
			case GLFW_KEY_ESCAPE: closeGame();                            break;
			case GLFW_KEY_W:			mainCharacter->moveForward(); 					break;
			case GLFW_KEY_Q:			mainCharacter->moveDown();							break;
			case GLFW_KEY_A:			mainCharacter->moveLeft();							break;
			case GLFW_KEY_D:			mainCharacter->moveRight();							break;
			case GLFW_KEY_S:			mainCharacter->moveBackward();					break;
			case GLFW_KEY_SPACE:	mainCharacter->moveUp();								break;
			case GLFW_KEY_N: std::cout << mainCharacter->xpos << ":" << mainCharacter->ypos << ":" << mainCharacter->zpos << "\n";
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

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
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	mainCharacter->mainCam.processMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mainCharacter->mainCam.processMouseScroll(yoffset);
}

void mousekey_callback(GLFWwindow* window, int button, int action, int mods)
{

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
      mainCharacter->destroyBlock();
  if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    mainCharacter->addBlock(1);
}

void moveForward(bool isOn)
{
	static bool on = false;
	on = isOn;

	if(on) mainCharacter->moveForward();
}

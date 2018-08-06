#define GLEW_STATIC
#include <GL/glew.h>
#include <map>
// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <math.h>
#include <CEGUI/CEGUI.h>

#include "../Character/include/mainchar.h"
#include "../MainEngine/include/world.h"
#include "../MainEngine/include/threads.h"
#include "../Settings/settings.h"

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
CEGUI::Key::Scan translateGLFWtoCEGUI(int key)
{
	//using namespace CEGUI;
	switch(key)
	{
		case GLFW_KEY_W: return CEGUI::Key::Scan::W;
		case GLFW_KEY_A: return CEGUI::Key::Scan::A;
		case GLFW_KEY_S: return CEGUI::Key::Scan::S;
		case GLFW_KEY_D: return CEGUI::Key::Scan::D;
		case GLFW_KEY_P: return CEGUI::Key::Scan::P;
		case GLFW_KEY_ESCAPE: return CEGUI::Key::Escape;
		case GLFW_KEY_SPACE: return CEGUI::Key::Space;
	}
}

void updateInputs()
{
	for(std::map<int,int>::iterator it=keyMap.begin(); it!=keyMap.end(); ++it)
	{

		int key = it->first;
		if(key == GLFW_KEY_ESCAPE)
		{
			closeGame();
		}
		mainCharacter->handleKeyHold(key);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		mainCharacter->handleKeyPress(key);
	}
	else if(action == GLFW_RELEASE)
	{
		mainCharacter->handleKeyRelease(key);
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
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	mainCharacter->processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mainCharacter->mainCam.processMouseScroll(yoffset);
}

void mousekey_callback(GLFWwindow* window, int button, int action, int mods)
{

	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
    mainCharacter->handleMouseClick(GLFW_MOUSE_BUTTON_LEFT);

		context.injectMouseButtonDown(CEGUI::LeftButton);
	}
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
		context.injectMouseButtonUp(CEGUI::LeftButton);
	}

  if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		mainCharacter->handleMouseClick(GLFW_MOUSE_BUTTON_RIGHT);
		CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
		context.injectMouseButtonDown(CEGUI::RightButton);
	}
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
		context.injectMouseButtonUp(CEGUI::RightButton);
	}
}

void moveForward(bool isOn)
{
	static bool on = false;
	on = isOn;

	if(on) mainCharacter->moveForward();
}

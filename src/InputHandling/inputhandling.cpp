#include "../headers/all.h"


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// mouse position variables
float lastX = 400;
float lastY = 300;
bool firstMouse = true;

MainChar* mainChar;

void initializeInputs(MainChar* mc)
{
	mainChar = mc;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainChar->moveForward();
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainChar->moveBackward();
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainChar->moveLeft();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainChar->moveRight();
  if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    mainChar->moveUp();
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    mainChar->moveDown();
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

	mainChar->mainCam.processMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mainChar->mainCam.processMouseScroll(yoffset);
}

void mousekey_callback(GLFWwindow* window, int button, int action, int mods)
{

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
      mainChar->destroyBlock();
  if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    mainChar->addBlock(1);
}

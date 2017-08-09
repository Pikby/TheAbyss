#include <string>
#include "headers/SOIL.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "headers/shaders.h"
#include "headers/camera.h"
#include "headers/bsp.h"

#include "headers/openGL.h"
#include "headers/mainchar.h"
#include "headers/text.h"
#include <assert.h>

//Global maincharacter reference which encapsulates the camera

World* newWorld;
MainChar* mainCharacter;
GLFWwindow* window;
CharRenderer* text;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// mouse position variables
float lastX = 400;
float lastY = 300;
bool firstMouse = true;


GLFWwindow* createWindow(int width, int height)
{
  //Initial windows configs

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  //Create the window
  window = glfwCreateWindow(width, height, "Prototype 1.000", nullptr, nullptr);

  return window;
}

void initWorld()
{
  //glfwMakeContextCurrent(window);
  newWorld = new World;
  mainCharacter = new MainChar(0,150,0,newWorld);
  text = new CharRenderer;
  newWorld->renderWorld(round(mainCharacter->xpos/16),round(mainCharacter->ypos/16),round(mainCharacter->zpos/16));
}

void* draw(void* )
{
  glfwMakeContextCurrent(window);
  long long totalFrame = 0;
  std::string fpsString;



  while(!glfwWindowShouldClose(window))
  {

    totalFrame++;
	   // update the delta time each frame
	 float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //Create a time for every second and displays the FPS
    float timer = glfwGetTime();
    float timerDec = timer - floor(timer);
    if(timerDec+deltaTime>=1.0f)
    {
      fpsString = "FPS: ";
      fpsString.append(std::to_string(totalFrame));
      totalFrame = 0;

    }

    glfwPollEvents();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mainCharacter->update();
    //newWorld->renderWorld(round(mainCharacter->xpos/16),round(mainCharacter->ypos/16),round(mainCharacter->zpos/16));
    //Draw and render the world centered around the player
    newWorld->drawWorld(round(mainCharacter->xpos/16),round(mainCharacter->ypos/16),round(mainCharacter->zpos/16),&(mainCharacter->mainCam));
    //std::cout << glGetError() << "update loop\n";
    text->RenderText(fpsString, 50.0f, 550.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    glfwSwapBuffers(window);
  }
}

void* render(void* )
{
  while(!glfwWindowShouldClose(window))
  {
    //newWorld->renderWorld(round(mainCharacter->xpos/16),round(mainCharacter->zpos/16));
  }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainCharacter->moveForward();
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainCharacter->moveBackward();
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainCharacter->moveLeft();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainCharacter->moveRight();
  if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    mainCharacter->jump();
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

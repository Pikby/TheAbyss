enum Type {STATIC,DYNAMIC,STREAM};

#include "headers/SOIL.h"
#include <iostream>
#include <math.h>
#include <vector>
// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Add the shader configs
#include "headers/shaders.h"
#include "headers/openGL.h"
#include "headers/block.h"
#include "headers/camera.h"
#include "headers/mainchar.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// create a camera using the camera class
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// mouse position variables
float lastX = 400;
float lastY = 300;
bool firstMouse = true;

int main()
{
  glfwInit();


  int winWidth = 800;
  int winHeight = 600;
  GLFWwindow* window = createWindow(winWidth,winHeight);

  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  //Intialize glew and all related settings
  glewExperimental = GL_TRUE;
  glewInit();
  glViewport(0, 0, winWidth, winHeight);
  glEnable(GL_DEPTH_TEST);



  std::vector <Block> levelBlocks;
  MainChar main(0,10,"../assets/textures/tilesf1.jpg");
  levelBlocks.push_back(Block(0.0f,0.0f,"../assets/textures/tilesf1.jpg",STATIC));

  while(!glfwWindowShouldClose(window))
  {
	// update the delta time each frame
	float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwPollEvents();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(int x = 0 ; x<levelBlocks.size();x++)
    {
      levelBlocks.at(x).draw(camera.GetViewMatrix());
    }

    main.draw(camera.GetViewMatrix());
    main.update(levelBlocks);
    glfwSwapBuffers(window);
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyBoard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyBoard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyBoard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyBoard(RIGHT, deltaTime);
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

	camera.ProcessMouseMovement(xoffset, yoffset, false);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

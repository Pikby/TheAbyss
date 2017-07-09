enum Type {STATIC,DYNAMIC,STREAM};

#include "headers/SOIL.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

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
#include "headers/block.h"
#include "headers/openGL.h"
#include "headers/camera.h"
#include "headers/mainchar.h"
#include "headers/text.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// create a camera using the camera class
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
MainChar* testMain;

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
  glfwSwapInterval(0);//FPS Capping
  //Intialize glew and all related settings
  glewExperimental = GL_TRUE;
  glewInit();
  glViewport(0, 0, winWidth, winHeight);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  std::vector <Block> blockIds;
  blockIds.push_back(Block("../assets/textures/tilesf1.jpg",0));
  blockIds.push_back(Block("../assets/textures/tilesf1.jpg",0));
  std::vector <WorldBlk> levelBlocks;
  levelBlocks.push_back(WorldBlk(0,0,0,&(blockIds.at(0))));
  levelBlocks.push_back(WorldBlk(1,1,1,&(blockIds.at(1))));

  for(int x = 0;x<100;x++)
  {
    for(int y = 0;y<100;y++)
    {
      levelBlocks.push_back(WorldBlk(x,0,y,&blockIds.at(0)));
    }
  }


  testMain = new MainChar(0,10,"../assets/textures/tilesf1.jpg",&levelBlocks);

  //Intializes the text rendering object
  CharRenderer text;

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

    //Render the fps string
    text.RenderText(fpsString, 50.0f, 550.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

    //Render each individual item on screen
    for(int x = 0 ; x<levelBlocks.size();x++)
    {
      levelBlocks.at(x).draw(camera.GetViewMatrix());
    }


    testMain->draw(camera.GetViewMatrix());
    testMain->update();



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
		testMain->moveLeft();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		testMain->moveRight();
  if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    testMain->jump();
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
